package controllers;

import models.CustomWebSocketActor;
import play.mvc.Controller;
import play.mvc.Result;
import play.mvc.WebSocket;
import views.html.index;

public class Application extends Controller {

	public static WebSocket<String> connectWebSocket() {
		return WebSocket.withActor(CustomWebSocketActor::props);
    }
	
	public static Result index() {
		return ok(index.render("Your new application is ready."));
	}

}
