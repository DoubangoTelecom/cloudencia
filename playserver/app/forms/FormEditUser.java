package forms;

import models.User;
import play.data.validation.Constraints.Email;
import play.data.validation.Constraints.EmailValidator;

public class FormEditUser {
	@Email
	private String email;
	private String friendlyName;
	private long id = -1L;
	private long developerId = -1L;
	
	public FormEditUser() {
		
	}
	
	public FormEditUser(final User user) {
		if (user != null) {
			id = user.id;
			email = user.email;
			friendlyName = user.friendlyName;
			if (user.developer != null) {
				developerId = user.developer.id;
			}
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
	
	public long getDeveloperId() {
		return developerId;
	}

	public void setDeveloperId(long developerId) {
		this.developerId = developerId;
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
