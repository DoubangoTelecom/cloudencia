package models;

import java.util.Map;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.MapKey;
import javax.persistence.OneToMany;

import play.Logger;
import play.data.validation.Constraints.Email;
import play.data.validation.Constraints.Required;
import play.db.ebean.Model;
import utils.Tools;

@Entity
public class Developer extends Model{
	private static final long serialVersionUID = 1L;
	
	@Required
	@Id
	public long id;
	
	@Required
	@Email
	@Column(nullable = false, unique = true)
	public String email;
	
	@Required
	public String password;
	
	@Required
	public String friendlyName;
	
	@Required
	public String licenseKey;
	
	@OneToMany(cascade = CascadeType.ALL, mappedBy="developer", fetch=FetchType.LAZY)
	@MapKey(name = "id")
	public Map<Long, User> users;
	
	@OneToMany(cascade = CascadeType.ALL, mappedBy="developer", fetch=FetchType.LAZY)
	@MapKey(name = "id")
	public Map<Long, IceServer> iceServers;
	
	public static Finder<Long, Developer> find = new Finder<Long, Developer>(
			Long.class, Developer.class
		  );
	
	public Developer() {
		password = Tools.randomPin();
		licenseKey = Tools.randomLicenseKey();
	}
	
	public static boolean create(Developer developer) {
		try {
			developer.save();
			return true;
		}
		catch(Exception e) {
			Logger.error("Failed to create developer", e);
		}
		return false;		
	}
	
	public static Developer findByEmail(String email) {
		return find.where().eq("email", email).findUnique();
	}
}
