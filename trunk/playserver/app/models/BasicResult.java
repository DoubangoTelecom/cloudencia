package models;

public class BasicResult {
	
	private final boolean mOk;
	private final String mReason;
	
	public final static BasicResult ok = new BasicResult(true);
	public final static BasicResult nok = new BasicResult(false);
	
	public BasicResult(boolean ok, String reason) {
		mOk = ok;
		mReason = reason;
	}

	public BasicResult(boolean ok) {
		mOk = ok;
		mReason = ok ? "ok" : "nok";
	}
	
	public boolean isOk() {
		return mOk;
	}
	
	public boolean isNok() {
		return !isOk();
	}

	public String getReason() {
		return mReason;
	}
	
	public static class BasicSuccess extends BasicResult {
		public BasicSuccess(String reason) {
			super(true, reason);
		}
		public BasicSuccess() {
			super(true);
		}
	}
	
	public static class BasicError extends BasicResult {
		public BasicError(String reason){
			super(false, reason);
		}
		public BasicError(){
			super(false);
		}
	}
}
