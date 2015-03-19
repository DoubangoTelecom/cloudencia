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
				final String errorMsgString = authenticateConn(jsonMessage.getFrom(), jsonMessage.getAuthToken());
				if (errorMsgString != null) {
					// authentication = NOK
					final JsonMsg.Error error = new JsonMsg.Error((short)403, errorMsgString);
					error.copyRequestToResponse(jsonMessage);
					mOut.tell(error.asText(), self());
					disconnect();
				}
				else {
					// authentication = OK
					final JsonMsg.Success success = new JsonMsg.Success((short)200, errorMsgString);
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
		// stop timers
		if (mConnAuth != null && !mConnAuth.isCancelled()) {
			mConnAuth.cancel();
		}
		if (mHeartbeat != null && !mHeartbeat.isCancelled()) {
			mHeartbeat.cancel();
		}
		// clear resources
		synchronized(actors) {
			if (mEmail != null) {
				actors.get(mEmail).remove(mId);
				if (actors.get(mEmail).isEmpty()) {
					actors.remove(mEmail);
				}
			}
		}
		super.postStop();
	}
	
	public boolean isConnAuthenticated() {
		return mConnAuthenticated;
	}
	
	public static HashMap<Long/*actorId*/, CustomWebSocketActor> getActors(String email) {
		synchronized(actors) {
			return actors.get(email);
		}
	}
	
	private synchronized static long getUniqueId() {
		return actorId++;
	}
	
	// returns error message if fails
	private String authenticateConn(final String email, final String authToken) {
		final User user = User.findByEmail(email);
		if (user == null) {
			return "Failed to find user with id = " + email;
		}
		mAuthToken = Tools.buildAuthToken(email, user.password);
		if (!mAuthToken.equalsIgnoreCase(authToken)) {
			return "Invalid authentication token for user with id = " + email;
		}
		mConnAuthenticated = true;
		return null;
	}
	
	private void disconnect() {
		self().tell(PoisonPill.getInstance(), self());
	}
}
