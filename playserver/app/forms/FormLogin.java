package forms;

import models.SessionInfo;
import play.Play;

public class FormLogin {
	
	private SessionInfo.Type type;
	private String name; // Email for user and dev
	private String password;
		
	public static String typeToString(FormLogin form) {
		return SessionInfo.typeToString(form.type);
	}
	
	public FormLogin() {
		type = SessionInfo.Type.Unknown;
	}
	
	public FormLogin(SessionInfo.Type _type, String _name, String _password) {
		type = _type;
		name = _name;
		password = _password;
	}
	
	public String validate() {
		switch (type) {
		case Admin:
			if (name != null && password != null 
			 && name.equalsIgnoreCase(Play.application().configuration().getString("admin.login"))
			 && password.equalsIgnoreCase(Play.application().configuration().getString("admin.password"))) {
				return null;
			}
			break;
		case Dev:
			break;
		case User:
			break;
		default:
			break;
		}
		 
		 return "Invalid password";
	 }
	
	public SessionInfo.Type getType() {
		return type;
	}
	public void setType(SessionInfo.Type type) {
		this.type = type;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
}
