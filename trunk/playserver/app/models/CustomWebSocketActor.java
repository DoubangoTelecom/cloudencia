package models;

import java.util.HashMap;
import java.util.concurrent.TimeUnit;

import models.Enums.ChatMessageStatus;
import play.Logger;
import play.libs.Akka;
import scala.concurrent.duration.Duration;
import utils.Consts;
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
		logDbg("New incomming WebSocket connection");
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
		logDbg("New incomming WebSocket message: " + message);
		if (message instanceof String) {
			final JsonMsg.PartiallyParsed jsonMsgPP = JsonMsg.PartiallyParsed.parse(message.toString());
			if (jsonMsgPP == null) {
				logErr("Failed to parse JSON content: " + message);
				mOut.tell(JsonMsg.Error.asText("Failed to parse JSON content: " + message), self());
				disconnect();
				return;
			}
			if (!mConnAuthenticated && jsonMsgPP.getHdrType() != JsonMsg.Hdr.TYPE.AUTH_CONN) {
				logErr("First message must be connection authentication but we found: " + message);
				mOut.tell(JsonMsg.Error.asText("First message must be connection authentication but we found: " + message), self());
				disconnect();
				return;
			}
			if (jsonMsgPP.getHdrType() == JsonMsg.Hdr.TYPE.AUTH_CONN) {
				if (mConnAuth != null && !mConnAuth.isCancelled()) {
					mConnAuth.cancel();
				}
				final BasicResult result = authenticateConn(jsonMsgPP.getHdr().getFrom(), jsonMsgPP.getHdr().getAuthToken());
				if (result.isNok()) {
					// authentication = NOK
					logErr("Autentication failed. Message = " + message + ", reason = " + result.getReason());
					final JsonMsg.Error error = new JsonMsg.Error(Consts.CODE_ERROR_FORBIDDEN, result.getReason());
					error.copyRequestToResponse(jsonMsgPP);
					mOut.tell(error.asText(), self());
					disconnect();
				}
				else {
					// authentication = OK
					logDbg("*** Autentication ok ***");
					final JsonMsg.Success success = new JsonMsg.Success(Consts.CODE_SUCCESS_OK, result.getReason());
					success.copyRequestToResponse(jsonMsgPP);
					mOut.tell(success.asText(), self());
				}
			}
			else if (jsonMsgPP.getHdrType() == JsonMsg.Hdr.TYPE.CHAT) {
				final BasicResult result = forwardMsgPP(jsonMsgPP);
				if (result.isNok()) {
					final JsonMsg.Error error = new JsonMsg.Error(Consts.CODE_ERROR_NOTSENT, result.getReason());
					error.copyRequestToResponse(jsonMsgPP);
					mOut.tell(error.asText(), self());
				}
				else {
					final JsonMsg.Success success = new JsonMsg.Success((short)200, result.getReason());
					success.copyRequestToResponse(jsonMsgPP);
					mOut.tell(success.asText(), self());
				}
			}
			else if (jsonMsgPP.getHdrType() == JsonMsg.Hdr.TYPE.SUCCESS || jsonMsgPP.getHdrType() == JsonMsg.Hdr.TYPE.ERROR) {
				boolean isError = (jsonMsgPP.getHdrType() == JsonMsg.Hdr.TYPE.ERROR);
				final BasicResult result = forwardMsgPP(jsonMsgPP);
				if (result.isNok()) {
					// FIXME: do something
				}
				else {
					final Ticket ticket = jsonMsgPP.getHdr().getTicket();
					if (ticket == null) {
						logWarn("No ticket in answer:" + message);
					}
					else {
						if (ticket.isTypeChat()) {
							if (isError) {
								
							}
							else {
								final short code = jsonMsgPP.get(Consts.JSON_FIELD_CODE, (short)0);
								if (code != 0) {
									final ChatMessage chatMsg = ChatMessage.findById(ticket.getId());
									if (chatMsg != null) {
										if (code == Consts.CODE_SUCCESS_DELIVERED) {
											chatMsg.setStatus(ChatMessageStatus.Delivered);
											ChatMessage.update(chatMsg);
										}
										else if (code == Consts.CODE_SUCCESS_SEEN) {
											chatMsg.setStatus(ChatMessageStatus.Seen);
											ChatMessage.update(chatMsg);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else {
			logErr("Invalid content type: " + message);
			mOut.tell(JsonMsg.Error.asText("Invalid content type: " + message), self());
			disconnect();
			return;
		}
	}
	
	@Override
	public void postStop() throws Exception {
		logDbg("Disconnecting WebSocket connection");
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
		logDbg("WebSocket connection disconnected");
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
	
	public BasicResult forwardMsgPP(final String dstEmail, final JsonMsg.PartiallyParsed jsonMsgPP) {
		final User dstUser = User.findByEmail(dstEmail);
		if (dstUser == null) {
			return new BasicResult.BasicError("User with email address = " + dstEmail + " not found");
		}
		// Store the message
		if (jsonMsgPP.getHdrType() == JsonMsg.Hdr.TYPE.CHAT) {
			final ChatMessage chatMsg = ChatMessage.build(jsonMsgPP);
			if (chatMsg != null) {
				if (!ChatMessage.create(chatMsg)) {
					return new BasicResult.BasicError("Failed to store chat message");
				}
			}
			else {
				return new BasicResult.BasicError("Failed to build chat message");
			}
			jsonMsgPP.addTicket(new Ticket.Chat(chatMsg.getId()));
		}
		
		final HashMap<Long/*actorId*/, CustomWebSocketActor> dstActors = getActors(dstEmail);
		if (dstActors == null || dstActors.isEmpty()) {
			// FIXME: push notifications and emails
		}
		else {
			BasicResult ret;
			for (HashMap.Entry<Long, CustomWebSocketActor> entry : dstActors.entrySet()) {
				ret = entry.getValue().forwardData(jsonMsgPP.getDataWithoutAuthToken());
				if (ret.isNok()) {
					return ret;
				}
			}
		}
		return BasicResult.ok;
	}
	
	public BasicResult forwardMsgPP(final JsonMsg.PartiallyParsed jsonMsgPP) {
		return forwardMsgPP(jsonMsgPP.getHdr().getTo(), jsonMsgPP);
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
				actor.logDbg("New actor added for email = " + actor.mEmail);
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
	
	private String logBuidMessage(String msg) {
		return "[Class=CustomWebSocketActor, Id = " + mId + "] - " + msg;
	}
	
	private void logDbg(String msg) {
		Logger.debug(logBuidMessage(msg));
	}
	
	private void logErr(String msg) {
		Logger.error(logBuidMessage(msg));
	}
	
	private void logWarn(String msg) {
		Logger.warn(logBuidMessage(msg));
	}
}
