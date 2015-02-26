import static org.fest.assertions.Assertions.assertThat;
import static play.mvc.Http.Status.OK;
import static play.test.Helpers.status;

import org.junit.Test;

import play.mvc.Result;
import play.test.FakeRequest;
import play.test.Helpers;

public class UserManipulationTest {
	@Test
    public void testCreate() {
		final String email = "noreply@doubango.org";

	    FakeRequest fakeRequest = new FakeRequest("POST", "/Users/Create/" + email);
	    
	    Result result = Helpers.callAction(controllers.routes.ref.Users.create(email, null), fakeRequest.withTextBody("test").withHeader("Test", "test"));
	    assertThat(status(result)).isEqualTo(OK);
    }
}
