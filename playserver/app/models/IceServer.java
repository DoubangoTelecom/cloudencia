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
public class IceServer  extends Model {
	private static final long serialVersionUID = 1L;
	
	public enum Protocol
	{
		UDP,
		TCP,
		TLS
	};
	
	@Id
	@Required
	public Long id;
	
	Protocol protocol;
	
	@Column(name = "enable_stun", columnDefinition = "boolean default true", nullable = false)
	public boolean enableStun;
	
	@Column(name = "enable_turn", columnDefinition = "boolean default false", nullable = false)
	public boolean enableTurn;
	
	@Column(nullable = false)
	public String host;
	
	@Column(nullable = false)
	public short port;
	
	@Column(nullable = true)
	public String login;
	
	@Column(nullable = true)
	public String password;
	
	@Required
	@ManyToOne(fetch=FetchType.LAZY)
	@JoinColumn(name="developer_id")
	public Developer developer;
}
