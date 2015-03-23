package models;

import java.util.HashMap;
import java.util.concurrent.TimeUnit;

import models.JsonMsg.TYPE;
import play.libs.Akka;
import scala.concurrent.duration.Duration;
import utils.Tools;
import akka.actor.ActorRef;
import akka.actor.Cancellable;
import akka.actor.PoisonPill;
import akka.actor.Props;
import akka.actor.UntypedActor;

public class CustomWebSocketActor extends UntypedActor {
	int mCount;
	boolean mConnAuthenticated;
	boolean mConnected;
	final ActorRef mOut;
	final long mId;
	String mEmail;
	String mAuthToken; // MD5 ('password' ':' 'login' ':' 'doubango.org')
	Cancellable mConnAuth;
	Cancellable mHeartbeat;
	
	static HashMap<String/*email*/, HashMap<Long/*actorId*/, CustomWebSocketActor> > actors = new HashMap<String, HashMap<Long, CustomWebSocketActor> >();
	static long actorId;
	
	public static Props props(ActorRef out) {
		return Props.create(CustomWebSocketActor.class, out);
	}

	public CustomWebSocketActor(ActorRef out) {
		mId = getUniqueId();
		mOut = out;
	}

	@Override
	public void preStart() throws Exception {
		mConnected = true;
		// Disconnect the socket if not authenticated after 3000ms
		// FIXME: the 3000ms must come from the application config
		mConnAuth = Akka.system().scheduler().scheduleOnce(
		        Duration.create(3000, TimeUnit.MILLISECONDS),
		        () -> { 
		        	if (!isConnAuthenticated()) {
		        		mOut.tell(JsonMsg.Error.asText("3000 milliseconds ellapsed and no authentication message received, closing the socket."), self());
		        		disconnect(); 
		        	}
		        	else {
		        		// heartbeat  timeouts
		        		// FIXME: the 3000ms must come from the application config
		        		if (true) { // FIXME: "heartbeat" must be enabled/disable using application config
		        			mHeartbeat = Akka.system().scheduler().schedule(
		        					Duration.create(0, TimeUnit.MILLISECONDS), // Initial delay 0 milliseconds
		        					Duration.create(3000, TimeUnit.MINUTES),     // Frequency xxxx milliseconds
		        			        () -> { 
		        			        	// FIXME: send PING
		        			        },
		        			        Akka.system().dispatcher()
		        			);
		        		}
		        	}
		        },
		        Akka.system().dispatcher()
		);
		super.preStart();
	}

	@Override
	public void onReceive(Object message) throws Exception {
		if (message instanceof String) {
			final JsonMsg jsonMessage = JsonMsg.parse(message.toString());
			if (jsonMessage == null) {
				mOut.tell(JsonMsg.Error.asText("Failed to parse JSON content: " + message), self());
				disconnect();
				return;
			}
			if (!mConnAuthenticated && jsonMessage.getType() != JsonMsg.TYPE.AUTH_CONN) {
				mOut.tell(JsonMsg.Error.asText("First message must be connection authentication but we found: " + message), self());
				disconnect();
				return;
			}
			if (jsonMessage.getType() == TYPE.AUTH_CONN) {
				if (mConnAuth != null && !mConnAuth.isCancelled()) {
					mConnAuth.cancel();
				}
				final BasicResult result = authenticateConn(jsonMessage.getFrom(), jsonMessage.getAuthToken());
				if (result.isNok()) {
					// authentication = NOK
					final JsonMsg.Error error = new JsonMsg.Error((short)403, result.getReason());
					error.copyRequestToResponse(jsonMessage);
					mOut.tell(error.asText(), self());
					disconnect();
				}
				else {
					// authentication = OK
					final JsonMsg.Success success = new JsonMsg.Success((short)200, result.getReason());
					success.copyRequestToResponse(jsonMessage);
					mOut.tell(success.asText(), self());
				}
			}
			else if (jsonMessage.getType() == TYPE.CHAT) {
				final BasicResult result = forwardData(jsonMessage.getTo(), message);
				if (result.isNok()) {
					final JsonMsg.Error error = new JsonMsg.Error((short)300, result.getReason());
					error.copyRequestToResponse(jsonMessage);
					mOut.tell(error.asText(), self());
				}
				else {
					final JsonMsg.Success success = new JsonMsg.Success((short)200, result.getReason());
					success.copyRequestToResponse(jsonMessage);
					mOut.tell(success.asText(), self());
				}
			}
		}
		else {
			mOut.tell(JsonMsg.Error.asText("Invalid content type: " + message), self());
			disconnect();
			return;
		}
	}
	
	@Override
	public void postStop() throws Exception {
		mConnected = false;
		// stop timers
		if (mConnAuth != null && !mConnAuth.isCancelled()) {
			mConnAuth.cancel();
		}
		if (mHeartbeat != null && !mHeartbeat.isCancelled()) {
			mHeartbeat.cancel();
		}
		// remove actor
		removeActor(this);
		// cleanup authentication fields
		mAuthToken = null;
		mEmail = null;
		mConnAuthenticated = false;
		super.postStop();
	}
	
	public boolean isConnAuthenticated() {
		return mConnAuthenticated;
	}
	
	public boolean isConnected() {
		return mConnected;
	}
	
	public BasicResult forwardData(final Object data) {
		if (data == null) {
			return new BasicResult.BasicError("Null message");
		}
		if (!isConnected()) {
			return new BasicResult.BasicError("Not Connected");
		}
		if (!isConnAuthenticated()) {
			return new BasicResult.BasicError("Connection not authenticated");
		}
		mOut.tell(data, self());
		return BasicResult.ok;
	}
	
	public BasicResult forwardData(final String dstEmail, final Object data) {
		final HashMap<Long/*actorId*/, CustomWebSocketActor> dstActors = getActors(dstEmail);
		if (dstActors == null || dstActors.isEmpty()) {
			// FIXME: store message
		}
		else {
			BasicResult ret;
			for (HashMap.Entry<Long, CustomWebSocketActor> entry : dstActors.entrySet()) {
				ret = entry.getValue().forwardData(data);
				if (ret.isNok()) {
					return ret;
				}
			}
		}
		return BasicResult.ok;
	}
	
	public BasicResult forwardMessage(final JsonMsg jsonMessage) {
		return forwardData(jsonMessage.asText());
	}
	
	private BasicResult forwardMessage(final String dstEmail, final JsonMsg jsonMessage) {
		return forwardData(dstEmail, jsonMessage);
	}
	
	public static HashMap<Long/*actorId*/, CustomWebSocketActor> getActors(String email) {
		synchronized(actors) {
			return actors.get(email);
		}
	}
	
	private static void addActor(CustomWebSocketActor actor) {
		if (actor != null && actor.mEmail != null) {
			synchronized(actors) {
				if (!actors.containsKey(actor.mEmail)) {
					actors.put(actor.mEmail, new HashMap<Long/*actorId*/, CustomWebSocketActor>());
				}
				actors.get(actor.mEmail).put(actor.mId, actor);
			}
		}
	}
	
	private static void removeActor(CustomWebSocketActor actor) {
		if (actor != null && actor.mEmail != null) {
			synchronized(actors) {
				if (actors.containsKey(actor.mEmail)) {
					actors.get(actor.mEmail).remove(actor.mId);
					if (actors.get(actor.mEmail).isEmpty()) {
						actors.remove(actor.mEmail);
					}
				}
			}
		}
	}
	
	private synchronized static long getUniqueId() {
		return actorId++;
	}
	
	private BasicResult authenticateConn(final String email, final String authToken) {
		final User user = User.findByEmail(email);
		if (user == null) {
			return new BasicResult.BasicError("Failed to find user with id = " + email);
		}
		mAuthToken = Tools.buildAuthToken(email, user.password);
		if (!mAuthToken.equalsIgnoreCase(authToken)) {
			return new BasicResult.BasicError("Invalid authentication token for user with id = " + email);
		}
		mEmail = email;
		mConnAuthenticated = true;
		// add actor
		addActor(this);
		return BasicResult.ok;
	}
	
	private void disconnect() {
		self().tell(PoisonPill.getInstance(), self());
	}
}
