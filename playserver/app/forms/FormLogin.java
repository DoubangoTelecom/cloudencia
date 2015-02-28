package forms;

import models.Developer;
import models.SessionInfo;
import models.User;
import play.Play;
import play.db.ebean.Model;

public class FormLogin {
	
	public SessionInfo.Type type;
	private String name; // Email for user and dev
	private String password;
	private Model actor; // Developer or User
		
	public static String typeToString(FormLogin form) {
		return SessionInfo.typeToString(form.type);
	}
	
	public FormLogin() {
		type = SessionInfo.Type.Unknown;
		 actor = null;
	}
	
	public FormLogin(SessionInfo.Type _type, String _name, String _password) {
		type = _type;
		name = _name;
		password = _password;
	}
	
	public String validate() {
		if (name != null && password != null ) {
			switch (type) {
			case Admin:
				if (name.equalsIgnoreCase(Play.application().configuration().getString("admin.login"))
				 && password.equalsIgnoreCase(Play.application().configuration().getString("admin.password"))) {
					return null;
				}
				break;
			case Dev:
				Developer developer = Developer.findByEmail(name);
				if (developer != null && developer.password.equals(password)) {
					actor = developer;
					return null;
				}
				break;
			case User:
				User user = User.findByEmail(name);
				if (user != null && user.password.equals(password)) {
					actor = user;
					return null;
				}
				break;
			default:
				break;
			}
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
	
	public Model getActor() {
		return actor;
	}
}
