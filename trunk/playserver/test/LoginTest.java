import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static play.test.Helpers.callAction;
import static play.test.Helpers.fakeApplication;
import static play.test.Helpers.fakeGlobal;
import static play.test.Helpers.fakeRequest;
import static play.test.Helpers.inMemoryDatabase;
import static play.test.Helpers.session;
import static play.test.Helpers.status;

import java.util.List;

import org.junit.Before;
import org.junit.Test;

import play.libs.Yaml;
import play.mvc.Result;
import play.test.WithApplication;

import com.avaje.ebean.Ebean;
import com.google.common.collect.ImmutableMap;

public class LoginTest extends WithApplication {
    @SuppressWarnings({ "rawtypes", "deprecation" })
	@Before
    public void setUp() {
        start(fakeApplication(inMemoryDatabase(), fakeGlobal()));
        Ebean.save((List) Yaml.load("test-data.yml"));
    }

    @Test
    public void adminAuthSuccess() {
        Result result = callAction(
            controllers.routes.ref.Application.postLogin(),
            fakeRequest().withFormUrlEncodedBody(ImmutableMap.of(
                "login", "bob@example.com",
                "password", "secret"))
        );
        assertEquals(302, status(result));
        assertEquals("bob@example.com", session(result).get("email"));
    }
    
    @Test
    public void adminAuthFailure() {
        Result result = callAction(
            controllers.routes.ref.Application.postLogin(),
            fakeRequest().withFormUrlEncodedBody(ImmutableMap.of(
                "login", "bob@example.com",
                "password", "badpassword"))
        );
        assertEquals(400, status(result));
        assertNull(session(result).get("login"));
    }
    
    @Test
    public void authenticated() {
        Result result = callAction(
            controllers.routes.ref.Application.index(),
            fakeRequest().withSession("email", "bob@example.com")
        );
        assertEquals(200, status(result));
    }
    
    @Test
    public void notAuthenticated() {
        Result result = callAction(
            controllers.routes.ref.Application.index(),
            fakeRequest()
        );
        assertEquals(303, status(result));
        // assertEquals("/login", header("Location", result));
    }
}