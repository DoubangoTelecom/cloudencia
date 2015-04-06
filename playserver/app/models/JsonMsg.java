package models;

import play.libs.Json;
import utils.Consts;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;

//
//	JsonMsg
//
public class JsonMsg {
	Hdr mHdr;
	
	JsonMsg() {
		this(null);
	}
	
	JsonMsg(final Hdr hdr) {
		mHdr = hdr;
	}
	
	public Hdr.TYPE getHdrType() {
		return mHdr == null ? Hdr.TYPE.UNKNOWN : mHdr.getType();
	}
	
	public Hdr getHdr() {
		return mHdr;
	}
	
	public void copyRequestToResponse(final Hdr requestHdr) {
		if (mHdr != null) {
			mHdr.copyRequestToResponse(requestHdr);
		}
	}
	
	public void copyRequestToResponse(final JsonMsg request) {
		if (mHdr != null & request.getHdr() != null) {
			mHdr.copyRequestToResponse(request.getHdr());
		}
	}
	
	public static JsonMsg parse(final String message) {
		final Hdr hr = Hdr.parse(message);
		if (hr != null) {
			final JsonMsg msg = new JsonMsg(hr);
			return msg;
		}
		return null;
	}
	
	//
	//	PartiallyParsed
	//
	public static class PartiallyParsed extends JsonMsg {
		String mData;
		JsonNode mDataJson;
		String mDataWithoutToken;
		
		PartiallyParsed(final Hdr hdr, final String data) {
			super(hdr);
			mData = data;
		}
		
		public String getData(boolean withouAuthToken) {
			if (withouAuthToken) {
				if (mDataWithoutToken == null && mData != null) {
					final JsonNode json = Json.parse(mData);
					if (json != null && json instanceof ObjectNode) {
						((ObjectNode)json).remove(Consts.JSON_FIELD_AUTH_TOKEN);
					}
					mDataWithoutToken = json.toString();
				}
				return mDataWithoutToken == null ? mData : mDataWithoutToken;
			}
			return mData;
		}
		
		public String getData() {
			return getData(false);
		}
		
		public JsonNode getDataJson() {
			if (mDataJson == null && mData != null) {
				mDataJson = Json.parse(mData);
			}
			return mDataJson;
		}
		
		public String getDataWithoutAuthToken() {
			return getData(true);
		}
		
		public static PartiallyParsed parse(final String message) {
			final Hdr hr = Hdr.parse(message);
			if (hr != null) {
				final PartiallyParsed msgData = new PartiallyParsed(hr, message);
				return msgData;
			}
			return null;
		}
		
		public void addTicket(long id, String type) {
			addTicket(new Ticket(id, type));
		}
		
		public void addTicket(Ticket ticket) {
			if (mHdr != null) {
				mHdr.addTicket(ticket);
			}
			if (getDataJson() != null) {
				if (mDataJson instanceof ObjectNode) {
					((ObjectNode)mDataJson).remove(Consts.JSON_FIELD_TICKET);
					if (ticket != null) {
						((ObjectNode)mDataJson).put(Consts.JSON_FIELD_TICKET, ticket.toJson());
					}
				}
				mData = mDataJson.toString();
			}
		}
		
		public short get(String field, short defaultVal) {
			if (getDataJson() != null) {
				final JsonNode node = getDataJson().get(field);
				if (node != null && node.isIntegralNumber()) {
					return (short)node.asInt((int)defaultVal);
				}
			}
			return defaultVal;
		}
	}
	
	//
	//	JsonMsgAnswer
	//
	public static abstract class Answer extends JsonMsg {
		String mReason;
		short mCode;
		
		public Answer(final Hdr.TYPE type, final short code, final String reason) {
			mHdr = new Hdr(type);
			mCode = code;
			mReason = reason;
		}
		
		public Answer(final Hdr.TYPE type, final String reason) {
			mHdr = new Hdr(type);
			mCode = 0;
			mReason = reason;
		}
		
		public static String asText(final String reason) {
			return new Error(reason).asText();
		}
		
		public String asText(boolean hideAuthToken) {
			final ObjectNode obj = mHdr.asObjectNode(hideAuthToken);
			if (mReason != null) {
				obj.put(Consts.JSON_FIELD_REASON, mReason);
			}
			if (mCode != 0) {
				obj.put(Consts.JSON_FIELD_CODE, mCode);
			}
			return obj.toString();
		}
		
		public String asText() {
			return asText(false);
		}
	}
	
	//
	//	JsonMsgError
	//
	public static class Error extends Answer {
		public Error(final String reason) {
			super(Hdr.TYPE.ERROR, reason);
		}
		public Error(final short code, final String reason) {
			super(Hdr.TYPE.ERROR, code, reason);
		}
	}
	
	//
	//	JsonMsgSuccess
	//
	public static class Success extends Answer {
		public Success(final String reason) {
			super(Hdr.TYPE.SUCCESS, reason);
		}
		public Success(final short code, final String reason) {
			super(Hdr.TYPE.SUCCESS, code, reason);
		}
	}
	
	//
	//	JsonMsgProvisional
	//
	public static class Provisional extends Answer {
		public Provisional(final String reason) {
			super(Hdr.TYPE.PROVISIONAL, reason);
		}
		public Provisional(final short code, final String reason) {
			super(Hdr.TYPE.PROVISIONAL, code, reason);
		}
	}
	
	//
	//	Hdr
	//
	public static class Hdr
	{
		public enum TYPE
		{
			UNKNOWN,
			ERROR,
			SUCCESS,
			PROVISIONAL,
			AUTH_CONN,
			CHAT
		}
		
		TYPE mType = TYPE.UNKNOWN;
		String mFrom;
		String mTo;
		String mCallId;
		String mTransacId;
		String mAuthToken;
		Ticket mTicket;
		
		Hdr() {
			this(TYPE.UNKNOWN);
		}
		
		Hdr(TYPE type) {
			mType = type;
		}
		
		public TYPE getType() {
			return mType;
		}

		public String getFrom() {
			return mFrom;
		}

		public String getTo() {
			return mTo;
		}

		public String getCallId() {
			return mCallId;
		}

		public String getTransacId() {
			return mTransacId;
		}

		public String getAuthToken() {
			return mAuthToken;
		}
		
		public Ticket getTicket() {
			return mTicket;
		}
		
		public void addTicket(long id, String type) {
			mTicket = new Ticket(id, type);
		}
		
		public void addTicket(Ticket ticket) {
			mTicket = ticket;
		}
		
		public ObjectNode asObjectNode(boolean hideAuthToken) {
			ObjectNode result = Json.newObject();
			if (mType != null) {
				result.put(Consts.JSON_FIELD_TYPE, typeToString(mType));
			}
			if (mFrom != null) {
				result.put(Consts.JSON_FIELD_FROM, mFrom);
			}
			if (mTo != null) {
				result.put(Consts.JSON_FIELD_TO, mTo);
			}
			if (mCallId != null) {
				result.put(Consts.JSON_FIELD_CID, mCallId);
			}
			if (mTransacId != null) {
				result.put(Consts.JSON_FIELD_TID, mTransacId);
			}
			if (!hideAuthToken) {
				if (mAuthToken != null) {
					result.put(Consts.JSON_FIELD_AUTH_TOKEN, mAuthToken);
				}
			}
			if (mTicket != null && mTicket.isValid()) {
				ObjectNode ticket = Json.newObject();
				ticket.put(Consts.JSON_FIELD_ID, mTicket.getId());
				ticket.put(Consts.JSON_FIELD_TYPE, mTicket.getType());
				result.put(Consts.JSON_FIELD_TICKET, ticket);
			}
			return result;
		}
		
		public ObjectNode asObjectNode() {
			return asObjectNode(false);
		}
		
		public String asText(boolean hideAuthToken) {
			final ObjectNode obj = asObjectNode(hideAuthToken);
			return obj.toString();
		}
		
		public String asText() {
			return asText(false);
		}
		
		public void copyRequestToResponse(final Hdr requestHdr) {
			if (requestHdr != null) {
				mFrom = requestHdr.mTo;
				mTo = requestHdr.mFrom;
				mCallId = requestHdr.mCallId;
				mTransacId = requestHdr.mTransacId;
				mTicket = requestHdr.mTicket;
			}
		}
		
		public static String typeToString(final TYPE type) {
			switch (type) {
			case ERROR: return Consts.JSON_FIELD_TYPE_ERROR;
			case SUCCESS: return Consts.JSON_FIELD_TYPE_SUCCESS;
			case PROVISIONAL: return Consts.JSON_FIELD_TYPE_PROVISIONAL;
			case AUTH_CONN: return Consts.JSON_FIELD_TYPE_AUTH_CONN;
			case CHAT: return Consts.JSON_FIELD_TYPE_CHAT;
			default: return "unknown";
			}
		}
		
		public static TYPE typeFromString(final String type) {
			if (type == null || type.isEmpty()) {
				return TYPE.UNKNOWN;
			}
			if (Consts.JSON_FIELD_TYPE_ERROR.equals(type)) {
				return TYPE.ERROR;
			}
			if (Consts.JSON_FIELD_TYPE_SUCCESS.equals(type)) {
				return TYPE.SUCCESS;
			}
			if (Consts.JSON_FIELD_TYPE_PROVISIONAL.equals(type)) {
				return TYPE.PROVISIONAL;
			}
			if (Consts.JSON_FIELD_TYPE_AUTH_CONN.equals(type)) {
				return TYPE.AUTH_CONN;
			}
			if (Consts.JSON_FIELD_TYPE_CHAT.equals(type)) {
				return TYPE.CHAT;
			}
			return TYPE.UNKNOWN;
		}
		
		public static boolean typeIsValid(final String type) {
			return typeFromString(type) != TYPE.UNKNOWN;
		}
		
		public static TYPE parseType(final String message) {
			if (message == null) {
				return TYPE.UNKNOWN;
			}
			final JsonNode json = Json.parse(message);
			if (json == null) {
				return TYPE.UNKNOWN;
			}
			final JsonNode _type = json.get(Consts.JSON_FIELD_TYPE);
			if (_type == null || !_type.isTextual() || _type.toString().isEmpty()) {
				return TYPE.UNKNOWN;
			}
			return typeFromString(_type.asText());
		}

		public static Hdr parse(final String message) {
			if (message == null) {
				return null;
			}
			final JsonNode json = Json.parse(message);
			if (json == null) {
				return null;
			}
			final JsonNode _type = json.get(Consts.JSON_FIELD_TYPE);
			if (_type == null || !_type.isTextual() || _type.toString().isEmpty()) {
				return null;
			}
			TYPE type = typeFromString(_type.asText());
			if (type == TYPE.UNKNOWN) {
				return null;
			}
			final JsonNode from = json.get(Consts.JSON_FIELD_FROM);
			if (from == null || !from.isTextual() || from.toString().isEmpty()) {
				return null;
			}
			final JsonNode to = json.get(Consts.JSON_FIELD_TO);
			if (to == null || !to.isTextual() || to.toString().isEmpty()) {
				if (type == TYPE.CHAT) { // requires "To field"
					return null;
				}
			}
			final JsonNode cid = json.get(Consts.JSON_FIELD_CID);
			if (cid == null || !cid.isTextual() || cid.toString().isEmpty()) {
				return null;
			}
			final JsonNode tid = json.get(Consts.JSON_FIELD_TID);
			if (tid == null || !tid.isTextual() || tid.toString().isEmpty()) {
				return null;
			}
			final JsonNode authToken = json.get(Consts.JSON_FIELD_AUTH_TOKEN);
			if (authToken == null || !authToken.isTextual() || authToken.toString().isEmpty()) {
				return null;
			}
			
			Hdr jsonHdr = new Hdr();
			jsonHdr.mType = type;
			jsonHdr.mFrom = from.asText();
			jsonHdr.mTo = (to == null) ? null : to.asText();
			jsonHdr.mCallId = cid.asText();
			jsonHdr.mTransacId = tid.asText();
			jsonHdr.mAuthToken = authToken.asText();
			
			final JsonNode ticket = json.get(Consts.JSON_FIELD_TICKET);
			if (ticket != null && ticket.isObject()) {
				final JsonNode ticketId = ticket.get(Consts.JSON_FIELD_ID);
				final JsonNode ticketType = ticket.get(Consts.JSON_FIELD_TYPE);
				if (ticketId != null && ticketId.isIntegralNumber() && ticketType != null && ticketType.isTextual()) {
					jsonHdr.mTicket = new Ticket(ticketId.asLong(), ticketType.asText());
				}
			}
			
			return jsonHdr;
		}
		
		public static boolean isValid(final String message) {
			return parse(message) != null;
		}
	}	
}
