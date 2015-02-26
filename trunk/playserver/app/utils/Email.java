package utils;

import models.BasicResult;

public class Email {

	class Result {
		public String reason;
		public boolean ok;
	}

	public static BasicResult send(String dstEmail, String subject, String body) {
		return BasicResult.ok;
	}
}
