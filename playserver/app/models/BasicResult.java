package models;

public class BasicResult {
	
	private final boolean mOk;
	private final String mReason;
	
	public final static BasicResult ok = new BasicResult(true);
	public final static BasicResult nok = new BasicResult(false);
	
	public BasicResult(boolean ok, String reason)
	{
		mOk = ok;
		mReason = reason;
	}

	public BasicResult(boolean ok)
	{
		mOk = ok;
		mReason = ok ? "ok" : "nok";
	}
	
	public boolean isOk() {
		return mOk;
	}

	public String getReason() {
		return mReason;
	}
}
