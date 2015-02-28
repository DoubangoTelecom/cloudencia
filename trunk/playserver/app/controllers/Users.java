package controllers;

import static play.libs.F.Promise.promise;
import models.BasicResult;
import models.User;
import play.data.validation.Constraints.EmailValidator;
import play.libs.F.Function;
import play.libs.F.Function0;
import play.libs.F.Promise;
import play.mvc.Controller;
import play.mvc.Result;
import utils.Email;



public class Users extends Controller {
	public static Promise<Result> create(String email, String name) {
		return promise(new Function0<BasicResult>() {
			public BasicResult apply() {
				// return Mail.send(email);
				return createIfDoesntExist(email, name);
			}
		}).map(new Function<BasicResult, Result>() {
			public Result apply(BasicResult basicResult) {
				if (basicResult.isOk()) {
					return ok(basicResult.getReason());
				}
				else {
					return internalServerError(basicResult.getReason());
				}
			}
		});
	}
	
	private static BasicResult createIfDoesntExist(String email, String name)
	{
		if (email == null || !new EmailValidator().isValid(email)) {
			return new BasicResult(false, "Invalid email: " + email);
		}
		User user = User.findByEmail(email);
		if (user == null) {
			user = new User();
			user.email = email;
			user.firendlyName = name;
			if (!User.create(user)) {
				return new BasicResult(false, "Faile to add new user  with email= " + email);
			}
		}
		return Email.send(email, "[Cloudencia] Your pin code", "Your pin code is...");
	}
}
