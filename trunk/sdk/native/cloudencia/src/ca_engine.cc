/* Copyright (C) 2011-2015 Mamadou DIOP
* Copyright (C) 2011-2015 Doubango Telecom <http://www.doubango.org>
*
* This file is part of Open Source Cloudendia WebRTC PaaS.
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
#include "cloudencia/ca_engine.h"

#include "tsk_debug.h"

#include "tinynet.h"

#include "tinydav/tdav.h"
#include "tinymedia.h"

#include <assert.h>

#if defined(_MSC_VER)
#	define snprintf		_snprintf
#	define vsnprintf	_vsnprintf
#	define strdup		_strdup
#	define stricmp		_stricmp
#	define strnicmp		_strnicmp
#else
#	if !HAVE_STRNICMP && !HAVE_STRICMP
#	define stricmp		strcasecmp
#	define strnicmp		strncasecmp
#	endif
#endif

bool CAEngine::s_bInitialized = false;

std::string CAEngine::s_strCredUserId = "";
std::string CAEngine::s_strCredPassword = "";

/**@defgroup _Group_CPP_Engine Engine
* @brief Static class used to configure the media and signaling engines.
*/


/*
* Constructor for the engine.
* Must never be called.
*/
CAEngine::CAEngine()
{

}

/*
* Destructor for the engine.
*/
CAEngine::~CAEngine()
{

}


/**@ingroup _Group_CPP_Engine
* Initializes the media and network layers. This function must be the first one to call.
* @param strCredUserId User (or device) identifer. This parameter is required and must not be null or empty.
* @param strCredPassword User (or device) password. This parameter is optional.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::init(std::string strCredUserId /*= ""*/, std::string strCredPassword /*= ""*/)
{
    if (!s_bInitialized) {
        if (tnet_startup() != 0) {
            return false;
        }

        if (tdav_init() != 0) {
            return false;
        }

        CA_ASSERT(tmedia_defaults_set_profile(tmedia_profile_rtcweb) == 0);
        CA_ASSERT(tmedia_defaults_set_avpf_mode(tmedia_mode_mandatory) == 0);
        CA_ASSERT(tmedia_defaults_set_srtp_type(tmedia_srtp_type_dtls) == 0);
        CA_ASSERT(tmedia_defaults_set_srtp_mode(tmedia_srtp_mode_mandatory) == 0);
        CA_ASSERT(tmedia_defaults_set_ice_enabled(tsk_true) == 0);

        CA_ASSERT(tmedia_defaults_set_pref_video_size(tmedia_pref_video_size_vga) == 0);
        CA_ASSERT(tmedia_defaults_set_video_fps(15) == 0);
#if CA_UNDER_WINDOWS
#else
        CA_ASSERT(tmedia_producer_set_friendly_name(tmedia_video, "/dev/video") == 0);
        CA_ASSERT(tmedia_producer_set_friendly_name(tmedia_bfcp_video, "/dev/video") == 0);
#endif

        CA_ASSERT(tmedia_defaults_set_echo_supp_enabled(tsk_true) == 0);
        CA_ASSERT(tmedia_defaults_set_echo_skew(0) == 0);
        CA_ASSERT(tmedia_defaults_set_echo_tail(100) == 0);

        CA_ASSERT(tmedia_defaults_set_opus_maxcapturerate(16000) == 0);
        CA_ASSERT(tmedia_defaults_set_opus_maxplaybackrate(16000) == 0);

        CA_ASSERT(tdav_set_codecs((tdav_codec_id_t)(tmedia_codec_id_vp8 | tmedia_codec_id_pcma | tmedia_codec_id_pcmu | tmedia_codec_id_opus)) == 0);
        CA_ASSERT(tdav_codec_set_priority((tdav_codec_id_t)tmedia_codec_id_vp8, 0) == 0);
        CA_ASSERT(tdav_codec_set_priority((tdav_codec_id_t)tmedia_codec_id_opus, 1) == 0);
        CA_ASSERT(tdav_codec_set_priority((tdav_codec_id_t)tmedia_codec_id_pcma, 2) == 0);
        CA_ASSERT(tdav_codec_set_priority((tdav_codec_id_t)tmedia_codec_id_opus, 3) == 0);

#if 0
        // Do not use BFCP signaling: Chrome will reject SDP with "m=application 56906 UDP/BFCP *\r\n"
        tmedia_session_plugin_unregister(tdav_session_bfcp_plugin_def_t);

        // Register fake display (Video consumer)
        CA_ASSERT(tmedia_consumer_plugin_register(sc_display_fake_plugin_def_t) == 0);
#endif

        s_bInitialized = true;
    }

    s_strCredUserId = strCredUserId;
    s_strCredPassword = strCredPassword;

    return true;
}

bool CAEngine::deInit()
{
    if (s_bInitialized) {
        tdav_deinit();
        tnet_cleanup();

        s_bInitialized = false;
    }

    return true;
}