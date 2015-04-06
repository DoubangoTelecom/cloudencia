package models;

import java.util.Date;

import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.Lob;
import javax.persistence.ManyToOne;
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Version;

import play.Logger;
import play.data.validation.Constraints.Required;
import play.db.ebean.Model;

@Entity
public class ChatMessage  extends Model{
	private static final long serialVersionUID = 1L;
	
	@Id
	@Required
	private long id;
	
	@Required
	@ManyToOne(fetch=FetchType.LAZY)
	@JoinColumn(name="from_id")
	private User from;
	
	@Required
	@ManyToOne(fetch=FetchType.LAZY)
	@JoinColumn(name="to_id")
	private User to;
	
	@Required
	private String callId;
	
	@Required
	private String transacId;
	
	@Required
	@Lob
	private String jsonContent;
	
	@Required
	private Date createdAt;
 
	@Required
	private Date updatedAt;
	
	@Required
	private Enums.ChatMessageStatus status = Enums.ChatMessageStatus.New;
	
	public static Finder<Long, ChatMessage> find = new Finder<Long, ChatMessage>(
			Long.class, ChatMessage.class
		  );
	
	public static ChatMessage findById(long id) {
		return find.byId(id);
	}
	
	public static ChatMessage build(final JsonMsg.PartiallyParsed msgPP) {
		if (msgPP.getHdrType() != JsonMsg.Hdr.TYPE.CHAT) {
			Logger.error("To build chat message you must provide a chat header");
			return null;
		}
		final JsonMsg.Hdr hdr = msgPP.getHdr();
		final ChatMessage msgChat = new ChatMessage();
		msgChat.from = User.findByEmail(hdr.getFrom());
		msgChat.to = User.findByEmail(hdr.getTo());
		msgChat.callId = hdr.getCallId();
		msgChat.transacId = hdr.getTransacId();
		msgChat.jsonContent = msgPP.getData();
		return msgChat;
	}
	
	public static boolean create(final ChatMessage message) {
		try {
			message.save();
			return true;
		}
		catch(Exception e) {
			Logger.error("Failed to create message" + e);
		}
		return false;		
	}
	
	public static boolean update(final ChatMessage message) {
		try {
			message.update();
			return true;
		}
		catch(Exception e) {
			Logger.error("Failed to update message" + e);
		}
		return false;		
	}
	
	@Override
	public void save() {
		createdAt();
		super.save();
	}
 
	@Override
  	public void update() {
	  updatedAt();
	  super.update();
  	}
 
	@PrePersist
  	void createdAt() {
	  this.createdAt = this.updatedAt = new Date();
  	}
 
	public long getId() {
		return id;
	}

	public User getFrom() {
		return from;
	}

	public User getTo() {
		return to;
	}

	public String getCallId() {
		return callId;
	}

	public String getTransacId() {
		return transacId;
	}

	public String getJsonContent() {
		return jsonContent;
	}

	public Enums.ChatMessageStatus getStatus() {
		return status;
	}
	
	public void setStatus(Enums.ChatMessageStatus newStatus) {
		status = newStatus;
	}

	@PreUpdate
  	void updatedAt() {
	  this.updatedAt = new Date();
  	}
}
