package utils;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Random;
import java.util.UUID;

import javax.xml.bind.annotation.adapters.HexBinaryAdapter;

public class Tools {
	public static final String emptyString = "";
	public static int randInt(int min, int max) {
	    return new Random().nextInt((max - min) + 1) + min;
	}
	
	public static String randomPin() {
		return String.format("%d%d%d%d", Tools.randInt(0, 9), Tools.randInt(0, 9), Tools.randInt(0, 9), Tools.randInt(0, 9));
	}
	
	public static String randomLicenseKey() {
		return UUID.randomUUID().toString().toUpperCase();
	}
	
	public static String randomUniquePhoneNumber(long userId) {
		String local = "";
		String userIdString = Long.toString(userId);
		int phoneMinLength = userIdString.length() < 9 ? 9 : userIdString.length();
		for (int i = userIdString.length(), j = 2; i < phoneMinLength; ++i, ++j) {
			local += j;
		}
		local += userIdString;
		for (int i = 0; i < local.length(); i += 3) {
			local += "-";
			++i;
		}
		return String.format("+1-000%s", local);
	}
	
	public static String buildAuthToken(final String login, final String password) {
		// auth-token = md5('password' ':' 'login' ':' 'doubango.org');
	    final String token  = password + ":" + login + ":" + "doubango.org";
	    final MessageDigest digest;
		try {
			digest = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
			return Tools.emptyString;
		}
	    return (new HexBinaryAdapter()).marshal(digest.digest(token.getBytes()));
	}
	
	public static String buildHa1(final String login, final String password, final String realm) {
		/* RFC 2617 - 3.2.2.2 A1
	    A1       = unq(username-value) ":" unq(realm-value) ":" passwd
	    */
	    final String ha1 = login + ":" + realm + ":" + password;
	    final MessageDigest digest;
		try {
			digest = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
			return Tools.emptyString;
		}
	    return (new HexBinaryAdapter()).marshal(digest.digest(ha1.getBytes()));
	}
}
