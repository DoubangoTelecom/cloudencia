package models;

import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;

import play.data.validation.Constraints.Required;
import play.db.ebean.Model;

@Entity
public class SipAccount extends Model{
	private static final long serialVersionUID = 1L;

	@Id
	@Required
	public long id;
	
	@Required
	public String address;
	
	@Required
	@ManyToOne(fetch=FetchType.LAZY)
	@JoinColumn(name="user_id")
	public User user;
}
