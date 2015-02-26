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
		return UUID.randomUUID().toString();
	}
}
