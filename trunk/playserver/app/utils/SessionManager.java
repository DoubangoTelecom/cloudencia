package utils;
import java.io.Serializable;

import play.libs.Json;
import play.mvc.Controller;

import com.fasterxml.jackson.databind.JsonNode;

public class SessionManager {
    public static<T extends Serializable> boolean addObject (String key, T value) {
    	// FIXME: log errors
    	try {
	    	if (key != null && value != null) {
	    		Controller.session(key, Json.toJson(value).toString());
	    		return true;
	    	}
    	}
    	catch(Exception e) {
    		
    	}
        return false;
    }

    public static <T extends Serializable> T getObject(String key, Class<T> c) {
    	// FIXME: log errors
    	try {
	    	if (key != null) {
	    		final String jsonValue = Controller.session(key);
	    		if (jsonValue != null) {
	    			final JsonNode jsonNode = Json.parse(jsonValue);
	    			if (jsonNode != null) {
	    				return Json.fromJson(jsonNode, c);
	    			}
	    		}
	    	}
    	}
    	catch(Exception e) {
    		
    	}
        return null;
    }
}
