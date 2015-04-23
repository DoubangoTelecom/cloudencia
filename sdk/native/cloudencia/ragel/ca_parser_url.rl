/* Copyright (C) 2011-2015 Mamadou DIOP
* Copyright (C) 2011-2015 Doubango Telecom <http://www.doubango.org>
*
* This file is part of Open Source Cloudencia WebRTC PaaS.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*/
#include "cloudencia/ca_parser_url.h"
#include "cloudencia/ca_url.h"
#include "cloudencia/ca_debug.h"

#include "tsk_ragel_state.h"
#include "tsk_string.h"
#include "tsk_memory.h"

%%{
	machine ca_machine_parser_url;

	# Includes
	include ca_machine_utils "./ragel/ca_machine_utils.rl";
			
	action tag{
		tag_start = p;
	}

	#/* Sets URL type */
	action is_tcp { scheme = tsk_strdup("tcp"), eType = CAUrlType_TCP; }
	action is_tls { scheme = tsk_strdup("tls"), eType = CAUrlType_TLS; }
	action is_http { scheme = tsk_strdup("http"), eType = CAUrlType_HTTP; }
	action is_https { scheme = tsk_strdup("https"), eType = CAUrlType_HTTPS; }
	action is_ws { scheme = tsk_strdup("ws"), eType = CAUrlType_WS; }
	action is_wss { scheme = tsk_strdup("wss"), eType = CAUrlType_WSS; }

	#/* Sets HOST type */
	action is_ipv4 { eHostType = CAUrlHostType_IPv4; }
	action is_ipv6 { eHostType = CAUrlHostType_IPv6; }
	action is_hostname { eHostType = CAUrlHostType_Hostname; }

	action parse_host{
		TSK_PARSER_SET_STRING(host);
	}

	action parse_port{
		have_port = tsk_true;
		TSK_PARSER_SET_INT(port);
	}

	action parse_hpath{
		TSK_PARSER_SET_STRING(hpath);
	}

	action parse_search{
		TSK_PARSER_SET_STRING(search);
	}

	action eob{
	}

	#// RFC 1738: "http://" hostport [ "/" hpath [ "?" search ]]
	#// FIXME: hpath is no optional (see above) but in my def. I use it as opt (any*).

	search = any* >tag %parse_search;
	hpath = any* >tag %parse_hpath;
	port = DIGIT+ >tag %parse_port;
	myhost = ((IPv6reference >is_ipv6) | (IPv4address >is_ipv4) | (hostname >is_hostname)) >tag %parse_host;
	hostport = myhost ( ":" port )?;
	main := ( (("tcp:"i>tag %is_tcp | "tls:"i>tag %is_tls | "ws:"i>tag %is_ws | "wss:"i>tag %is_wss | "http:"i>tag %is_http | "https:"i>tag %is_https) "//")? hostport? :>("/" hpath :>("?" search)?)? ) @eob;
	#main := ( hostport? :>("/" hpath :>("?" search)?)? ) @eob;
	
}%%

CAObjWrapper<CAUrl*> ca_url_parse(const char *urlstring, size_t length)
{
	CAObjWrapper<CAUrl*> oUrl;
	tsk_bool_t have_port = tsk_false;
	int cs = 0;
	const char *p = urlstring;
	const char *pe = p + length;
	const char *eof = pe;

	const char *ts = 0, *te = 0;
	int act = 0;
	
	const char *tag_start = 0;

	CAUrlType_t eType = CAUrlType_None;
	CAUrlHostType_t eHostType = CAUrlHostType_None;
	char* scheme = tsk_null;
	char* host = tsk_null;
	char* hpath = tsk_null;
	char* search = tsk_null;
	unsigned short port = 0;
	
	TSK_RAGEL_DISABLE_WARNINGS_BEGIN()
	%%write data;
	(void)(ts);
	(void)(te);
	(void)(act);
	(void)(eof);
	(void)(ca_machine_parser_url_first_final);
	(void)(ca_machine_parser_url_error);
	(void)(ca_machine_parser_url_en_main);
	%%write init;
	%%write exec;
	TSK_RAGEL_DISABLE_WARNINGS_END()
	
	if ( cs < %%{ write first_final; }%% ){
		CA_DEBUG_ERROR("Failed to parse URL: '%.*s'", length, urlstring);
		goto bail;
	}
	else if (!have_port) {
		if (eType == CAUrlType_HTTPS || eType == CAUrlType_WSS || eType == CAUrlType_TLS) {
			port = 443;
		}
		else {
			port = 80;
		}
	}

	oUrl = CAUrl::newObj(eType, scheme, host, hpath, search, port, eHostType);
	
bail:
	TSK_FREE(scheme);
	TSK_FREE(host);
	TSK_FREE(hpath);
	TSK_FREE(search);

	return oUrl;
}
