package controllers;

import models.CustomWebSocketActor;
import models.Developer;
import models.SessionInfo;
import models.SessionInfo.Type;
import models.User;
import play.data.Form;
import play.mvc.Controller;
import play.mvc.Result;
import play.mvc.Security;
import play.mvc.WebSocket;
import utils.SessionManager;
import views.html.index;
import forms.FormEditDeveloper;
import forms.FormEditUser;
import forms.FormLogin;

public class Application extends Controller {

	public static WebSocket<String> connectWebSocket() {
		return WebSocket.withActor(CustomWebSocketActor::props);
    }
	
	public static Result index() {
		return ok(index.render("Your new application is ready."));
	}
	
	public static Result home() {		
		return ok(views.html.page_home.render());
	}
	
	public static Result logout() {
	    session().clear();
	    flash("success", "You've been logged out");
	    return redirect(routes.Application.home());
	}
	
	@Security.Authenticated(Secured.SecuredAdmin.class)
	public static Result bootstrap() {
		return ok(views.html.bootstrap.render("salut comment tu vas?"));
	}
	
	/*-----------------
	 * 
	 * AUTHENTICATION
	 * 
	 ******************/
	
	public static Result getLogin(String loginType) {
		SessionInfo.Type type = SessionInfo.typeFromString(loginType);
		if (SessionInfo.Type.Unknown == type) {
			return badRequest();
		}
		final Form<FormLogin> form = Form.form(FormLogin.class).bindFromRequest().fill(new FormLogin(type, session().get("login.name"), session().get("login.password")));
		return ok(views.html.page_login.render(form));
	}
	
	public static Result postLogin() {
		final Form<FormLogin> form = Form.form(FormLogin.class).bindFromRequest();
		session().clear();
	    if (form.hasErrors()) {
	    	return badRequest(views.html.page_login.render(form));
	    }
	    else {
			if (form.get().type == Type.Dev) {
				final Developer developer = form.get().getActor() == null ? Developer.findByEmail(form.get().getName()) : (Developer)form.get().getActor();
				if (developer == null) {
					return badRequest(views.html.page_login.render(form));// FIXME: message
				}
				if (!SessionManager.addObject("login.devObj", developer)) {
					return badRequest(views.html.page_login.render(form)); // FIXME: message
				}
				session("login.devId", Long.toString(developer.id));
			}
			else if (form.get().type == Type.Dev) {
				final User user = form.get().getActor() == null ? User.findByEmail(form.get().getName()) : (User)form.get().getActor();
				if (user == null) {
					return badRequest(views.html.page_login.render(form));// FIXME: message
				}
				if (!SessionManager.addObject("login.userObj", user)) {
					return badRequest(views.html.page_login.render(form)); // FIXME: message
				}
				session("login.userId", Long.toString(user.id));
			}
			
			session("login.name", form.get().getName());
			session("login.password", form.get().getPassword());
			session("login.type", FormLogin.typeToString(form.get()));
	        return redirect(controllers.routes.Application.home());
	    }
	}
	
	/*-----------------
	 * 
	 * DEVELOPERS
	 * 
	 ******************/
	
	@Security.Authenticated(Secured.SecuredAdmin.class)
	public static Result getDevList() {
		return ok(views.html.page_dev_list.render(Developer.find.all()));
	}
	
	@Security.Authenticated(Secured.SecuredAdmin.class)
	public static Result getDevEdit(long id) {
		final Form<FormEditDeveloper> form;
		final Developer developer = (id == -1L) ? null : Developer.find.byId(id);
		form = Form.form(FormEditDeveloper.class).bindFromRequest().fill(new FormEditDeveloper(developer));
		return ok(views.html.page_dev_edit.render(form));
	}
	
	@Security.Authenticated(Secured.SecuredAdmin.class)
	public static Result postDevEdit() {
		final Form<FormEditDeveloper> form = Form.form(FormEditDeveloper.class).bindFromRequest();
		if (form.hasErrors()) {
	    	return badRequest(views.html.page_dev_edit.render(form));
	    }
	    else {
			Developer developer = (form.get().getId() == -1L) ? null : Developer.find.byId(form.get().getId());
			if (developer == null) {				
				developer = new Developer();
				developer.email = form.get().getEmail();
				developer.friendlyName = form.get().getFriendlyName();
				if (!Developer.create(developer)) {
					return badRequest(views.html.page_dev_edit.render(form));
				}
			}
			
			return ok(views.html.page_dev_list.render(Developer.find.all()));
	    }
	}

	
	
	/*-----------------
	 * 
	 * USERS
	 * 
	 ******************/
	
	@Security.Authenticated(Secured.SecuredDev.class)
	public static Result getUserList() {
		return ok(views.html.page_user_list.render(User.findByDeveloper(Long.parseLong(session("login.devId")))));
	}
	
	@Security.Authenticated(Secured.SecuredDev.class)
	public static Result getUserEdit(long id) {
		final Form<FormEditUser> form;
		final User user = (id == -1L) ? null : User.find.byId(id);
		form = Form.form(FormEditUser.class).bindFromRequest().fill(new FormEditUser(user));
		return ok(views.html.page_user_edit.render(form));
	}
	
	@Security.Authenticated(Secured.SecuredDev.class)
	public static Result postUserEdit() {
		final Form<FormEditUser> form = Form.form(FormEditUser.class).bindFromRequest();
		if (form.hasErrors()) {
	    	return badRequest(views.html.page_user_edit.render(form));
	    }
	    else {
			User user = (form.get().getId() == -1L) ? null : User.find.byId(form.get().getId());
			if (user == null) {
				final Developer developer = Developer.find.byId(form.get().getDeveloperId());
				if (developer == null) {
					return badRequest(views.html.page_user_edit.render(form));
				}
				user = new User();
				user.email = form.get().getEmail();
				user.friendlyName = form.get().getFriendlyName();
				user.developer = developer;
				if (!User.create(user)) {
					return badRequest(views.html.page_user_edit.render(form));
				}
			}
			return ok(views.html.page_user_list.render(User.findByDeveloper(form.get().getDeveloperId())));
	    }
	}
	
}
