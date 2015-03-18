package models;

import play.libs.Json;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;

public class JsonMsg {
	public static final String FIELD_TYPE = "type";
	public static final String FIELD_FROM = "from";
	public static final String FIELD_TO = "to";
	public static final String FIELD_CID = "cid";
	public static final String FIELD_TID = "tid";
	public static final String FIELD_AUTH_TOKEN = "authToken";
	public static final String FIELD_REASON = "reason";
	
	public static final String FIELD_TYPE_ERROR = "error";
	public static final String FIELD_TYPE_AUTH_CONN = "authConn";
	
	public enum TYPE
	{
		UNKNOWN,
		ERROR,
		AUTH_CONN
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
	
	public ObjectNode asObjectNode() {
		ObjectNode result = Json.newObject();
		if (mType != null) {
			result.put(FIELD_TYPE, typeToString(mType));
		}
		if (mFrom != null) {
			result.put(FIELD_FROM, mFrom);
		}
		if (mTo != null) {
			result.put(FIELD_TO, mTo);
		}
		if (mCallId != null) {
			result.put(FIELD_CID, mCallId);
		}
		if (mTransacId != null) {
			result.put(FIELD_TID, mTransacId);
		}
		if (mAuthToken != null) {
			result.put(FIELD_AUTH_TOKEN, mAuthToken);
		}
		return result;
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
		case ERROR: return FIELD_TYPE_ERROR;
		case AUTH_CONN: return FIELD_TYPE_AUTH_CONN;
		default: return "unknown";
		}
	}
	
	public static TYPE typeFromString(final String type) {
		if (type == null || type.isEmpty()) {
			return TYPE.UNKNOWN;
		}
		if (FIELD_TYPE_ERROR.equals(type)) {
			return TYPE.ERROR;
		}
		if (FIELD_TYPE_AUTH_CONN.equals(type)) {
			return TYPE.AUTH_CONN;
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
		final JsonNode _type = json.get(FIELD_TYPE);
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
		final JsonNode _type = json.get(FIELD_TYPE);
		if (_type == null || !_type.isTextual() || _type.toString().isEmpty()) {
			return null;
		}
		TYPE type = typeFromString(_type.asText());
		if (type == TYPE.UNKNOWN) {
			return null;
		}
		final JsonNode from = json.get(FIELD_FROM);
		if (from == null || !from.isTextual() || from.toString().isEmpty()) {
			return null;
		}
		final JsonNode cid = json.get(FIELD_CID);
		if (cid == null || !cid.isTextual() || cid.toString().isEmpty()) {
			return null;
		}
		final JsonNode tid = json.get(FIELD_TID);
		if (tid == null || !tid.isTextual() || tid.toString().isEmpty()) {
			return null;
		}
		final JsonNode authToken = json.get(FIELD_AUTH_TOKEN);
		if (authToken == null || !authToken.isTextual() || authToken.toString().isEmpty()) {
			return null;
		}
		JsonMsg jsonMessage = new JsonMsg();
		jsonMessage.mType = type;
		jsonMessage.mFrom = from.asText();
		jsonMessage.mCallId = cid.asText();
		jsonMessage.mTransacId = tid.asText();
		jsonMessage.mAuthToken = authToken.asText();
		return jsonMessage;
	}
	
	public static boolean isValid(final String message) {
		return parse(message) != null;
	}
	
	
	//
	//	JsonMsgError
	//
	public static class Error extends JsonMsg {
		String mReason;
		
		public Error(final String reason) {
			mType = TYPE.ERROR;
			mReason = reason;
		}
		
		public static String asText(final String reason) {
			return new Error(reason).asText();
		}
		
		public String asText() {
			final ObjectNode obj = asObjectNode();
			if (mReason != null) {
				obj.put(FIELD_REASON, mReason);
			}
			return obj.toString();
		}
	}
}
