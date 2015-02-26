package models;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;

import play.data.validation.Constraints.Required;
import play.db.ebean.Model;


@Entity
public class Phone extends Model{
	private static final long serialVersionUID = 1L;
	
	@Id
	@Required
	public Long id;
	
	@Required
	@Column(nullable = false)
	public String number;
	
	@Required
	@ManyToOne(fetch=FetchType.LAZY)
	@JoinColumn(name="contact_id")
	public Contact contact;
}
