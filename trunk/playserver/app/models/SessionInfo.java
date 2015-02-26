package models;


public class SessionInfo {
	
	public enum Type
	{
		Unknown,
		Admin,
		Dev,
		User
	}
	
	public SessionInfo() {
		
	}
	
	public static Type typeFromString(String type) {
		if (type != null) {
			if (type.equalsIgnoreCase("admin")) return Type.Admin;
			else if (type.equalsIgnoreCase("dev")) return Type.Dev;
			else if (type.equalsIgnoreCase("user")) return Type.User;
		}
		return Type.Unknown;
	}
	
	public static String typeToString(Type type) {
		switch (type) {
			case Admin: return "admin";
			case Dev: return "dev";
			case User: return "user";
			default: return "unknown";
		}
	}
}
