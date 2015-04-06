package models;

public class Enums {
	public static enum SessionType {
		Unknwown,
		Admin,
		Dev,
		User
	}
	
	// Used in the database, must not change the order
	public static enum ChatMessageStatus {
		New,
		Sent,
		Delivered,
		Seen
	}
}
