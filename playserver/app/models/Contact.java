package models;

import java.util.Map;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.MapKey;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;

import play.data.validation.Constraints.Required;
import play.db.ebean.Model;


@Entity
public class Contact extends Model{
	private static final long serialVersionUID = 1L;
	
	public enum Type
	{
		Email,
		Phone,
		PostalAddress
	};
	
	@Id
	@Required
	public long id;
	
	@Required
	public Type type;
	
	public String firstName;
	
	public String lastName;
	
	public String friendlyName;
	
	@OneToOne(optional = true)
	@Column(nullable = true)
	public User user;
	
	@OneToMany(cascade = CascadeType.ALL, mappedBy="contact", fetch=FetchType.LAZY)
	@MapKey(name = "id")
	public Map<Long, Phone> phoneNumbers;
}
