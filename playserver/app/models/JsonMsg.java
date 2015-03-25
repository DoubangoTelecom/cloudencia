package models;

import play.libs.Json;
import utils.Consts;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class JsonMsg {
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
	
	public void copyRequestToResponse(final JsonMsg request) {
		if (request != null) {
			mFrom = request.mTo;
			mTo = request.mFrom;
			mCallId = request.mCallId;
			mTransacId = request.mTransacId;
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

	public static JsonMsg parse(final String message) {
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
		
		JsonMsg jsonMessage = new JsonMsg();
		jsonMessage.mType = type;
		jsonMessage.mFrom = from.asText();
		jsonMessage.mTo = (to == null) ? null : to.asText();
		jsonMessage.mCallId = cid.asText();
		jsonMessage.mTransacId = tid.asText();
		jsonMessage.mAuthToken = authToken.asText();
		
		return jsonMessage;
	}
	
	public static boolean isValid(final String message) {
		return parse(message) != null;
	}
	
	//
	//	JsonMsgAnswer
	//
	public static abstract class Answer extends JsonMsg {
		String mReason;
		short mCode;
		
		public Answer(final TYPE type, final short code, final String reason) {
			mType = type;
			mCode = code;
			mReason = reason;
		}
		
		public Answer(final TYPE type, final String reason) {
			mType = type;
			mCode = 0;
			mReason = reason;
		}
		
		public static String asText(final String reason) {
			return new Error(reason).asText();
		}
		
		public String asText(boolean hideAuthToken) {
			final ObjectNode obj = asObjectNode(hideAuthToken);
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
			super(TYPE.ERROR, reason);
		}
		public Error(final short code, final String reason) {
			super(TYPE.ERROR, code, reason);
		}
	}
	
	//
	//	JsonMsgSuccess
	//
	public static class Success extends Answer {
		public Success(final String reason) {
			super(TYPE.SUCCESS, reason);
		}
		public Success(final short code, final String reason) {
			super(TYPE.SUCCESS, code, reason);
		}
	}
	
	//
	//	JsonMsgProvisional
	//
	public static class Provisional extends Answer {
		public Provisional(final String reason) {
			super(TYPE.PROVISIONAL, reason);
		}
		public Provisional(final short code, final String reason) {
			super(TYPE.PROVISIONAL, code, reason);
		}
	}
}
