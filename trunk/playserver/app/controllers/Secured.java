package controllers;

import models.SessionInfo;
import play.mvc.Http.Context;
import play.mvc.Result;
import play.mvc.Security;

public class Secured {
	
	public static abstract class SecuredBase extends Security.Authenticator {
	
		protected boolean isAdmin(Context ctx) {
			return SessionInfo.typeToString(SessionInfo.Type.Admin).equalsIgnoreCase(ctx.session().get("login.type"));
		}
		
		protected boolean isDev(Context ctx) {
			return SessionInfo.typeToString(SessionInfo.Type.Dev).equalsIgnoreCase(ctx.session().get("login.type"));
		}
		
		protected boolean isUser(Context ctx) {
			return SessionInfo.typeToString(SessionInfo.Type.User).equalsIgnoreCase(ctx.session().get("login.type"));
		}
		
		protected String getLoginName(Context ctx) {
			return ctx.session().get("login.name");
		}
		
		@Override
	    public abstract String getUsername(Context ctx);
	
	    @Override
	    public abstract Result onUnauthorized(Context ctx);
	}

	public static class SecuredAdmin extends SecuredBase {
		@Override
	    public String getUsername(Context ctx) {
	        if(isAdmin(ctx)) {
	        	return getLoginName(ctx);
	        }
	        return null;
	    }
	    @Override
	    public Result onUnauthorized(Context ctx) {
	        return redirect(routes.Application.getLogin(SessionInfo.typeToString(SessionInfo.Type.Admin)));
	    }
	}
	
	public static class SecuredDev extends SecuredBase {
		@Override
	    public String getUsername(Context ctx) {
	        if(isDev(ctx) || isAdmin(ctx)) {
	        	return getLoginName(ctx);
	        }
	        return null;
	    }
	    @Override
	    public Result onUnauthorized(Context ctx) {
	        return redirect(routes.Application.getLogin(SessionInfo.typeToString(SessionInfo.Type.Dev)));
	    }
	}
	
	public static class SecuredUser extends SecuredBase {
		@Override
	    public String getUsername(Context ctx) {
			return getLoginName(ctx);
	    }
	    @Override
	    public Result onUnauthorized(Context ctx) {
	        return redirect(routes.Application.getLogin(SessionInfo.typeToString(SessionInfo.Type.User)));
	    }
	}
}


