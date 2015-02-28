package utils;

import java.util.Random;
import java.util.UUID;

public class Tools {
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
}
