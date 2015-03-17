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
#include "cloudencia/ca_engine.h"
#include "cloudencia/ca_utils.h"

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
std::list<CAObjWrapper<CAIceServer*> > CAEngine::s_listIceServers;
CAObjWrapper<CAMutex* > CAEngine::s_listIceServersMutex = new CAMutex();

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
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::init()
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


/**@ingroup _Group_CPP_Engine
* Sets the debug level.
* @param eLevel The new debug level.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setDebugLevel(CADebugLevel_t eLevel)
{
    tsk_debug_set_level((int)eLevel);
    return true;
}

/**@ingroup _Group_CPP_Engine
* Sets the paths to the SSL certificates used to secure the signaling (WebSocket) an media (DTLS-SRTP) layers.
* The library comes with default self-signed certificates: <b>SSL_Pub.pem</b>, <b>SSL_Priv.pem</b> and <b>SSL_CA.pem</b>. These certificates could be used for testing.
* @param strPublicKey Path (relative or absolute) to the public SSL key (PEM format) used for both DTLS-SRTP (media) and WSS (signaling) connections. <b>Required</b>.
* @param strPrivateKey Path (relative or absolute) to the private SSL key (PEM format) used for both DTLS-SRTP (media) and WSS (signaling) connections. <b>Required</b>.
* @param strCA Path (relative or absolute) to the certificate authority used to sign the private and public keys. Used for both DTLS-SRTP (media) and WSS (signaling) connections. <b>Required</b>.
* @param bMutualAuth Whether to enable mutual authentication (apply to the signaling only).
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setSSLCertificates(std::string strPublicKey /*= ""*/, std::string strPrivateKey /*= ""*/, std::string strCA /*= ""*/, bool bMutualAuth /*= false*/)
{
#if CA_UNDER_WINDOWS
    char ssl_file_priv[MAX_PATH] = { '\0' };
    char ssl_file_pub[MAX_PATH] = { '\0' };
    char ssl_file_ca[MAX_PATH] = { '\0' };

    if (!strPublicKey.empty()) {
        if (!CAUtils::fileExists(strPublicKey.c_str())) {
            snprintf(ssl_file_pub, sizeof(ssl_file_pub), "%s/%s", CAUtils::currentDirectoryPath(), strPublicKey);
            strPublicKey = ssl_file_pub;
        }
    }
    if (!strPrivateKey.empty()) {
        if (!CAUtils::fileExists(strPrivateKey.c_str())) {
            snprintf(ssl_file_priv, sizeof(ssl_file_priv), "%s/%s", CAUtils::currentDirectoryPath(), strPrivateKey);
            strPrivateKey = ssl_file_priv;
        }
    }
    if (!strCA.empty()) {
        if (!CAUtils::fileExists(strCA.c_str())) {
            snprintf(ssl_file_ca, sizeof(ssl_file_ca), "%s/%s", CAUtils::currentDirectoryPath(), strCA);
            strCA = ssl_file_ca;
        }
    }
#endif /* CA_UNDER_WINDOWS */

    return (tmedia_defaults_set_ssl_certs(
                strPrivateKey.empty() ? NULL : strPrivateKey.c_str(),
                strPublicKey.empty() ? NULL : strPublicKey.c_str(),
                strCA.empty() ? NULL : strCA.c_str(),
                bMutualAuth ? tsk_true : tsk_false) == 0);
}

/**@ingroup _Group_CPP_Engine
* Sets the default prefered video size.
* @param strPrefVideoSize The prefered size. Accepted values:<br />
* "sqcif"(128x98)<br />
* "qcif"(176x144)<br />
* "qvga"(320x240)<br />
* "cif"(352x288)<br />
* "hvga"(480x320)<br />
* "vga"(640x480)<br />
* "4cif"(704x576)<br />
* "wvga"(800x480)<br />
* "svga"(800x600)<br />
* "480p"(852x480)<br />
* "720p"(1280x720)<br />
* "16cif"(1408x1152)<br />
* "1080p"(1920x1080)<br />
* "2160p"(3840x2160)<br />
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoPrefSize(std::string strPrefVideoSize)
{
    CA_ASSERT(!strPrefVideoSize.empty());
    int i;
    struct pref_video_size {
        const char* name;
        tmedia_pref_video_size_t size;
        size_t w;
        size_t h;
    };
    static const pref_video_size pref_video_sizes[] = {
        { "sqcif", tmedia_pref_video_size_sqcif, 128, 98 }, // 128 x 98
        { "qcif", tmedia_pref_video_size_qcif, 176, 144 }, // 176 x 144
        { "qvga", tmedia_pref_video_size_qvga, 320, 240 }, // 320 x 240
        { "cif", tmedia_pref_video_size_cif, 352, 288 }, // 352 x 288
        { "hvga", tmedia_pref_video_size_hvga, 480, 320 }, // 480 x 320
        { "vga", tmedia_pref_video_size_vga, 640, 480 }, // 640 x 480
        { "4cif", tmedia_pref_video_size_4cif, 704, 576 }, // 704 x 576
        { "wvga", tmedia_pref_video_size_wvga, 800, 480 }, // 800 x 480
        { "svga", tmedia_pref_video_size_svga, 800, 600 }, // 800 x 600
        { "xga", tmedia_pref_video_size_xga, 1024, 768 }, // 1024 x 768
        { "480p", tmedia_pref_video_size_480p, 852, 480 }, // 852 x 480
        { "720p", tmedia_pref_video_size_720p, 1280, 720 }, // 1280 x 720
        { "16cif", tmedia_pref_video_size_16cif, 1408, 1152 }, // 1408 x 1152
        { "1080p", tmedia_pref_video_size_1080p, 1920, 1080 }, // 1920 x 1080
        { "2160p", tmedia_pref_video_size_2160p, 3840, 2160 }, // 3840 x 2160
    };
    static const int pref_video_sizes_count = sizeof(pref_video_sizes) / sizeof(pref_video_sizes[0]);

    for (i = 0; i < pref_video_sizes_count; ++i) {
        if (tsk_striequals(pref_video_sizes[i].name, strPrefVideoSize.c_str())) {
            if (tmedia_defaults_set_pref_video_size(pref_video_sizes[i].size) == 0) {
                return true;
            }
        }
    }
    CA_DEBUG_ERROR("%s not valid as video size. Valid values: ...", strPrefVideoSize);
    return false;
}

/**@ingroup _Group_CPP_Engine
* Sets the default video frame rate.
* @param fps The frame rate (frames per second). Range: [1, 120].
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoFps(int fps)
{
    return ((tmedia_defaults_set_video_fps(fps) == 0));
}

/**@ingroup _Group_CPP_Engine
* Sets the maximum bandwidth (kbps) to use for outgoing video stream. <br />
* If congestion control is enabled then, the bandwidth will be updated based on the network conditions but these new values will never be higher than what you defined using this function.
* @param bandwwidthMax The new maximum bandwidth. Negative value means "compute best value".
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoBandwidthUpMax(int bandwwidthMax)
{
    return ((tmedia_defaults_set_bandwidth_video_upload_max(bandwwidthMax) == 0));
}

/**@ingroup _Group_CPP_Engine
* Sets the maximum bandwidth (kbps) to use for incoming video stream. <br />
* If congestion control is enabled then, the bandwidth will be updated based on the network conditions but these new values will never be higher than what you defined using this function.
* @param bandwwidthMax The new maximum bandwidth. Negative value means "compute best value".
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoBandwidthDownMax(int bandwwidthMax)
{
    return ((tmedia_defaults_set_bandwidth_video_download_max(bandwwidthMax) == 0));
}

/**@ingroup _Group_CPP_Engine
* Sets the video type. Supported values: 1 (low, e.g. home video security systems), 2 (medium, e.g conference call) or 3 (high, e.g. basketball game). <br />
* Formula:
* @code
* [[video-max-upload-bandwidth (kbps) = ((video-width * video-height * video-fps * motion-rank * 0.07) / 1024)]]
* @endcode
* @param motionRank New motion rank value.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoMotionRank(int motionRank)
{
    return ((tmedia_defaults_set_video_motion_rank(motionRank) == 0));
}

/**@ingroup _Group_CPP_Engine
* Defines whether to adapt the bandwidth usage based on the congestion (packet loss/retransmission/delay). This mainly use non-standard algorithms and partially implements draft-alvestrand-rtcweb-congestion-03 and draft-alvestrand-rmcat-remb-01.
* @param congestionCtrl Enable/disable congestion control. Default: <b>false</b>.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoCongestionCtrlEnabled(bool congestionCtrl)
{
    return ((tmedia_defaults_set_congestion_ctrl_enabled(congestionCtrl ? tsk_true : tsk_false) == 0));
}

/**@ingroup _Group_CPP_Engine
* Whether to enable or disable video jitter buffer. It's highly recommended to enable video jitter buffer because it's required to have RTCP-FB (NACK, FIR, PLI... as per RFC 5104) fully functional. Enabling video jitter buffer gives better quality and improves smoothness. For example, no RTCP-NACK messages will be sent to request dropped RTP packets if this option is disabled. It’s also up to the jitter buffer to reorder RTP packets. <br />
* In "phase 1", this is useless as we always have half-duplex (From device to Browser) video.
* @param enabled Enable/disable video jitter buffer. Default: <b>true</b>.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoJbEnabled(bool enabled)
{
    return ((tmedia_defaults_set_videojb_enabled(enabled ? tsk_true : tsk_false) == 0));
}

/**@ingroup _Group_CPP_Engine
* Defines the maximum and minimum queue length used to store the outgoing RTP packets. The stored packets are used to honor incoming RTCP-NACK requests. Check the technical documentation for more information. <br />
* In "phase 1", this is useless as we always have half-duplex (From device to Browser) video.
* @param min The minimum value. Default: 20.
* @param max The manimum value. Default 160.
*/
bool CAEngine::setVideoAvpfTail(int min, int max)
{
    return ((tmedia_defaults_set_avpf_tail(min, max) == 0));
}

/**@ingroup _Group_CPP_Engine
* Artifacts are introduced in video stream when RTP packets are lost. Enabling zero-artifact feature fix this issue (see technical guide for more information).
* In "phase 1", this is useless as we always have half-duplex (From device to Browser) video.
* @param enabled Enable/disable zero-artifact feature. Default: <b>true</b>.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setVideoZeroArtifactsEnabled(bool enabled)
{
    return ((tmedia_defaults_set_video_zeroartifacts_enabled(enabled ? tsk_true : tsk_false) == 0));
}

/**@ingroup _Group_CPP_Engine
* Whether to enable or disable audio echo suppression (AEC). It's highly recommended to enable this feature. <br />
* @param enabled Enable/disable audio echo suppression. Default: <b>true</b>.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setAudioEchoSuppEnabled(bool enabled)
{
    return ((tmedia_defaults_set_echo_supp_enabled(enabled ? tsk_true : tsk_false) == 0));
}

/**@ingroup _Group_CPP_Engine
* Defines the maximum tail length (in milliseconds) used for echo cancellation (AEC module). Value must be in <b>]0-500]</b> and should be <b>100</b> <br />
* @param tailLength The echo tail value. Default: 100.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setAudioEchoTail(int tailLength)
{
    return(tmedia_defaults_set_echo_tail(tailLength) == 0);
}

/**@ingroup _Group_CPP_Engine
* Adds a new ICE server to try when gathering candidates. You can add as many servers as you want. The servers will be tried in FIFT (First In First to Try) order. <br />
* Available since <b>1.0.1</b>.
* @param strTransportProto The transport protocol. Must be <b>udp</b>, <b>tcp</b> or <b>tls</b>. This parameter is case-insensitive.
* @param strServerHost ICE server hostname or IP address.
* @param serverPort ICE server port.
* @param useTurn Whether to use this ICE server to gather relayed candidates.
* @param useStun Whether to use this ICE server to gather reflexive candidates.
* @param strUsername Username used for long-term credentials. Required for TURN allocation.
* @param strPassword Password used for long-term credentials. Required for TURN allocation.
* @sa @ref clearNattIceServers()
*/
bool CAEngine::addNattIceServer(std::string strTransportProto, std::string strServerHost, unsigned short serverPort, bool useTurn /*= false*/, bool useStun /*= true*/, std::string strUsername /*= ""*/, std::string strPassword /*= ""*/)
{
    if (strTransportProto.empty() || strServerHost.empty() || !serverPort || (!useTurn && !useStun)) {
        CA_DEBUG_ERROR("Invalid paramter");
        return false;
    }
    if (!tsk_striequals(strTransportProto.c_str(), "udp") && !tsk_striequals(strTransportProto.c_str(), "tcp") && !tsk_striequals(strTransportProto.c_str(), "tls")) {
        CA_DEBUG_ERROR("'%s' not valid as ICE server transport protocol", strTransportProto.c_str());
        return false;
    }

    CAObjWrapper<CAIceServer* >iceServer = new CAIceServer(
        strTransportProto,
        strServerHost,
        serverPort,
        useTurn,
        useStun,
        strUsername,
        strPassword);
    if (iceServer) {
        CAEngine::s_listIceServersMutex->lock();
        CAEngine::s_listIceServers.push_back(iceServer);
        CAEngine::s_listIceServersMutex->unlock();
        return true;
    }
    return false;
}

/**@ingroup _Group_CPP_Engine
* Clears (removes) all ICE servers added using @ref addNattIceServer() <br />
* Available since <b>1.0.1</b>.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
* @sa @ref addNattIceServer()
*/
bool CAEngine::clearNattIceServers()
{
    CAEngine::s_listIceServersMutex->lock();
    CAEngine::s_listIceServers.clear();
    CAEngine::s_listIceServersMutex->unlock();
    return true;
}

/**@ingroup _Group_CPP_Engine
* Defines whether to gather ICE reflexive candidates.
* @param enabled
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setNattIceStunEnabled(bool enabled)
{
    return ((tmedia_defaults_set_icestun_enabled(enabled ? tsk_true : tsk_false) == 0));
}

/**@ingroup _Group_CPP_Engine
* Defines whether to gather ICE relayed candidates.
* @param enabled
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CAEngine::setNattIceTurnEnabled(bool enabled)
{
    return ((tmedia_defaults_set_iceturn_enabled(enabled ? tsk_true : tsk_false) == 0));
}
