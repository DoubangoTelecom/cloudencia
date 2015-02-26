package forms;

import models.BasicResult;
import models.Developer;
import play.data.validation.Constraints.Email;
import play.data.validation.Constraints.EmailValidator;

public class FormEditDeveloper {
	@Email
	private String email;
	private String friendlyName;
	private long id = -1L;
	
	public FormEditDeveloper() {
		
	}
	
	public FormEditDeveloper(final Developer developer) {
		if (developer != null) {
			id = developer.id;
			email = developer.email;
			friendlyName = developer.friendlyName;
		}
	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getFriendlyName() {
		return friendlyName;
	}

	public void setFriendlyName(String friendlyName) {
		this.friendlyName = friendlyName;
	}

	public long getId() {
		return id;
	}

	public void setId(long id) {
		this.id = id;
	}
	
	public String validate() {
		if (email == null || email.isEmpty() || !new EmailValidator().isValid(email)) {
			return "Invalid email: " + email;
		}
		if (friendlyName == null || friendlyName.isEmpty()) {
			return "Invalid friendly name: " + friendlyName;
		}
		return null;
	}
}
