package models;

import java.util.Map;

import javax.persistence.CascadeType;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.MapKey;
import javax.persistence.OneToMany;

import play.Logger;
import play.data.validation.Constraints.Email;
import play.data.validation.Constraints.Required;
import play.db.ebean.Model;
import utils.Tools;

@Entity
public class User extends Model {
	private static final long serialVersionUID = 1L;

	@Id
	@Required
	public long id;
	
	@Required
	@Email
	public String email;
	
	@Required
	public String name;
	
	public String icon;
	
	@Required
	public String password;
	
	@Required
	@ManyToOne(fetch=FetchType.LAZY)
	@JoinColumn(name="developer_id")
	public Developer developer;
	
	@OneToMany(cascade = CascadeType.ALL, mappedBy="user", fetch=FetchType.LAZY)
	@MapKey(name = "id")
	public Map<Long, SipAccount> sipAcounts;
	
	public User() {
		password = Tools.randomPin();
	}
	
	public static Finder<Long, User> find = new Finder<Long, User>(
			Long.class, User.class
		  );
	
	public static boolean create(User user) {
		try {
			user.save();
			return true;
		}
		catch(Exception e) {
			Logger.error("Failed to create user", e);
		}
		return false;		
	}
	
	public static User findByEmail(String email) {
		return find.where().like("email", "%"+email+"%").findUnique();
	}
}
