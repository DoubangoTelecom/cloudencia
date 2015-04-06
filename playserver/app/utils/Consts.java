package utils;

public class Consts {
	public static final long ID_TICKET_INVALID = 0;
	
	public static final short CODE_PROVISIONAL_TRYING = 100;
	public static final short CODE_PROVISIONAL_RINGING = 101;

	public static final short CODE_SUCCESS_OK = 200;
	public static final short CODE_SUCCESS_SENT = 201;
	public static final short CODE_SUCCESS_DELIVERED = 202;
	public static final short CODE_SUCCESS_SEEN = 203;

	public static final short CODE_ERROR_NOK = 300;
	public static final short CODE_ERROR_FORBIDDEN = 301;
	public static final short CODE_ERROR_NOTFOUND = 302;
	public static final short CODE_ERROR_NOTSENT = 303;
	
	public static final String JSON_FIELD_ID = "id";
	public static final String JSON_FIELD_TYPE = "type";
	public static final String JSON_FIELD_FROM = "from";
	public static final String JSON_FIELD_TO = "to";
	public static final String JSON_FIELD_CID = "cid";
	public static final String JSON_FIELD_TID = "tid";
	public static final String JSON_FIELD_AUTH_TOKEN = "authToken";
	public static final String JSON_FIELD_REASON = "reason";
	public static final String JSON_FIELD_CODE = "code";
	public static final String JSON_FIELD_TICKET = "ticket";
	
	public static final String JSON_FIELD_TYPE_ERROR = "error";
	public static final String JSON_FIELD_TYPE_SUCCESS = "success";
	public static final String JSON_FIELD_TYPE_PROVISIONAL = "provisional";
	public static final String JSON_FIELD_TYPE_AUTH_CONN = "authConn";
	public static final String JSON_FIELD_TYPE_CHAT = "chat";
}
