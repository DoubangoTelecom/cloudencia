package models;

import play.libs.Json;

import com.fasterxml.jackson.databind.node.ObjectNode;

import utils.Consts;

public class Ticket {
	final long mId;
	final String mType;
	
	public Ticket(long id, String type) {
		mId = id;
		mType = type;
	}
	
	public long getId() {
		return mId;
	}
	
	public String getType() {
		return mType;
	}
	
	public ObjectNode toJson() {
		if (isValid()) {
			ObjectNode ticket = Json.newObject();
			ticket.put(Consts.JSON_FIELD_ID, mId);
			ticket.put(Consts.JSON_FIELD_TYPE, mType);
			return ticket;
		}
		return null;
	}
	
	public boolean isValid() {
		return mId != Consts.ID_TICKET_INVALID && mType != null && !mType.isEmpty();
	}
	
	public boolean isTypeChat() {
		return Consts.JSON_FIELD_TYPE_CHAT.equals(mType);
	}
	
	public static class Chat extends Ticket {
		public Chat(long id) {
			super(id, Consts.JSON_FIELD_TYPE_CHAT);
		}
	}
}
