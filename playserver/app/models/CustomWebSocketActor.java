package models;

import akka.actor.ActorRef;
import akka.actor.Props;
import akka.actor.UntypedActor;

public class CustomWebSocketActor extends UntypedActor {
	int mCount;
	
	public static Props props(ActorRef out) {
		return Props.create(CustomWebSocketActor.class, out);
	}

	private final ActorRef out;

	public CustomWebSocketActor(ActorRef out) {
		this.out = out;
	}

	public void onReceive(Object message) throws Exception {
		if (message instanceof String) {
			out.tell("I received your message(" + ++mCount + "): " + message, self());
		}
	}
	
	@Override
	public void postStop() throws Exception {
		// clear resources
		super.postStop();
	}
}
