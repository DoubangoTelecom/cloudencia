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
#include "cloudencia/ca_api.h"

#include <assert.h>
#include <stdio.h>
#include <map>
#if !CA_UNDER_WINDOWS_CE
#include <fcntl.h>
#endif /* !CA_UNDER_WINDOWS_CE */

#if CA_UNDER_WINDOWS
#include <windows.h>
#endif /* CA_UNDER_WINDOWS */

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
#endif /* _MCA_VER */

static CAJson::Value jsonConfig;
static CAObjWrapper<CASessionCall*>callSession;
static CAObjWrapper<CASignaling*>signalSession;
static CAObjWrapper<CASignalingCallEvent*>pendingOffer;
static CAObjWrapper<CAThread*>threadConsoleReader;
static std::map<std::string/*CallId*/, std::map<std::string/*TransacId*/, CAObjWrapper<CAResultTransac* > > >chatMessages; // ((chatMessages[CallId])[TransacId]) == CAResultTransac

#if CA_UNDER_WINDOWS
#define WM_CA_ATTACH_DISPLAYS	MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 0x0201)
static DWORD mainThreadId = 0;
#define DEFAULT_VIDEO_REMOTE_WINDOW_NAME	L"Remote video window (Decoded RTP)" // Remote window is where the decoded video frames are displayed
#define DEFAULT_VIDEO_LOCAL_WINDOW_NAME		L"Local video window (Preview)" // Local window is where the encoded video frames are displayed before sending (preview, PIP mode).
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif /* CA_UNDER_WINDOWS */

#if !defined(MAX_PATH)
#	if defined(PATH_MAX)
#		define MAX_PATH PATH_MAX
#	else
#		define MAX_PATH 260
#	endif
#endif /* MAX_PATH */

static void printHelp();
static bool loadConfig();
static char config_file_path[MAX_PATH < 260 ? 260 : MAX_PATH] = { "./config.json" };

static void* CA_STDCALL consoleReaderProc(void *arg);
static bool attachDisplays();
static CAVideoDisplay getDisplay(bool bRemote, bool bScreenCast = false);

static CAVideoDisplay displayVideoLocal = NULL;
static CAVideoDisplay displayVideoRemote = NULL;
static CAVideoDisplay displayScreenCastLocal = NULL;
static CAVideoDisplay displayScreenCastRemote = NULL;
static bool connected = false;

#define kCAMobuleNameTest "CA Test"

class CASessionCallIceCallbackDummy : public CASessionCallIceCallback
{
public:
    CASessionCallIceCallbackDummy() {
    }
    virtual ~CASessionCallIceCallbackDummy() {
        CA_DEBUG_INFO_EX(kCAMobuleNameTest, "*** CASessionCallIceCallbackDummy destroyed ***");
    }
    virtual CA_INLINE const char* getObjectId() {
        return "CASessionCallIceCallbackDummy";
    }
    virtual bool onStateChanged(CAObjWrapper<CASessionCall* > oCall) {
        CA_DEBUG_INFO_EX(kCAMobuleNameTest, "ICE new state = %d", oCall->getIceState());
        return true;
    }
    static CAObjWrapper<CASessionCallIceCallback*> newObj() {
        return new CASessionCallIceCallbackDummy();
    }
};


class CASignalingCallbackDummy : public CASignalingCallback
{
protected:
    CASignalingCallbackDummy() {

    }
public:
    virtual ~CASignalingCallbackDummy() {
        CA_DEBUG_INFO_EX(kCAMobuleNameTest, "*** CASignalingCallbackDummy destroyed ***");
    }
	virtual CA_INLINE const char* getObjectId() {
		return "CASignalingCallbackDummy";
	}

	virtual bool onEventNet(const CAObjWrapper<CASignalingEvent* >& e) {
        //!\Deadlock issue: You must not call any function from 'CASignaling' class unless you fork a new thread.
        switch (e->getType()) {
        case CASignalingEventType_NetReady: {
            connected = true;
            CA_DEBUG_INFO_EX(kCAMobuleNameTest, "***Signaling module connected ***");
            break;
        }
        case CASignalingEventType_Error: {
            CA_DEBUG_INFO_EX(kCAMobuleNameTest, "***Signaling module error: %s ***", e->getDescription().c_str());
            break;
        }
        case CASignalingEventType_NetDisconnected:
        case CASignalingEventType_NetError: {
            connected = false;
            CA_DEBUG_INFO_EX(kCAMobuleNameTest, "***Signaling module disconnected ***");
            break;
        }
        case CASignalingEventType_NetData: {
            CA_DEBUG_INFO_EX(kCAMobuleNameTest, "***Signaling module passthrough DATA:%.*s ***", e->getDataSize(), (const char*)e->getDataPtr());
            break;
        }
        }

        return true;
    }
    
	virtual bool onEventResultTransac(const CAObjWrapper<CASignalingResultTransacEvent* >& e) {
		const CAObjWrapper<CAResultTransac* >& oResult = e->getResult();
		CA_ASSERT(oResult);
		CA_DEBUG_INFO_EX(kCAMobuleNameTest, "onEventResultTransac(code = %u, callId = %s, transacId = %s)", oResult->getCode(), oResult->getCallId().c_str(), oResult->getTransacId().c_str());
		// map result to sent chat messages
		if (chatMessages.find(oResult->getCallId()) != chatMessages.end() && chatMessages[oResult->getCallId()].find(oResult->getTransacId()) != chatMessages[oResult->getCallId()].end()) {
			CA_DEBUG_INFO_EX(kCAMobuleNameTest, "response code %u can be mapped to a chat message :)", oResult->getCode());
		}
		return true;
	}

	virtual bool onEventCall(const CAObjWrapper<CASignalingCallEvent* >& e) {
        //!\Deadlock issue: You must not call any function from 'CASignaling' class unless you fork a new thread.
        if (callSession) {
            if (callSession->getCallId() != e->getCallId()) {
                CA_DEBUG_ERROR("Call id mismatch: '%s'<>'%s'", callSession->getCallId().c_str(), e->getCallId().c_str());
                return CASessionCall::rejectEvent(signalSession, e);
            }
            bool ret = callSession->acceptEvent(e);
            if (e->getType() == "hangup") {
#if 1
                callSession = NULL;
                CA_DEBUG_INFO("+++Call ended +++");
#else		// auto-call
                CA_DEBUG_INFO("+++ call('%s',%d) +++", "002", CAMediaType_ScreenCast);
                CA_ASSERT(callSession = CASessionCall::newObj(signalSession));
                CA_ASSERT(callSession->setIceCallback(CASessionCallIceCallbackDummy::newObj()));
                CA_ASSERT(callSession->call(CAMediaType_ScreenCast, "002"));
                CA_ASSERT(attachDisplays());
#endif
            }
            return ret;
        }
        else {
            if (e->getType() == "offer") {
                if (callSession || pendingOffer) { // already in call?
                    return CASessionCall::rejectEvent(signalSession, e);
                }
                pendingOffer = e;
                CA_DEBUG_INFO_EX(kCAMobuleNameTest, "+++Incoming call: 'accept'/'reject'? +++");
            }
            if (e->getType() == "hangup") {
                if (pendingOffer && pendingOffer->getCallId() == e->getCallId()) {
                    pendingOffer = NULL;
                    CA_DEBUG_INFO_EX(kCAMobuleNameTest, "+++ pending call cancelled +++");
                }
            }

            // Silently ignore any other event type
        }

        return true;
    }

    static CAObjWrapper<CASignalingCallback*> newObj() {
        return new CASignalingCallbackDummy();
    }
};

#if defined(_WIN32_WCE) || defined(UNDER_CE)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    CA_ASSERT(threadConsoleReader = CAThread::newObj(consoleReaderProc));

    printf("*******************************************************************\n"
           "Copyright (C) 2011-2015 Doubango Telecom (VoIP division)\n"
           "PRODUCT: CLOUDENCIA (WebRTC PaaS client native SDK)\n"
           "HOME PAGE: <void>\n"
           "CODE SOURCE: <void>\n"
           "LICENCE: <void>\n"
           "VERSION: %s\n"
           "'quit' to quit the application.\n"
           "*******************************************************************\n\n"
           , CA_VERSION_STRING);

    /* load configuration */
    CA_ASSERT(loadConfig());

    CA_ASSERT(CAEngine::init());
    CA_ASSERT(CAEngine::setDebugLevel(jsonConfig["debug_level"].isNumeric() ? (CADebugLevel_t)jsonConfig["debug_level"].asInt() : CADebugLevel_Info));

#if 0
    static const char* __entries[] = {
        "debug_level",
        "ssl_file_pub", "ssl_file_priv", "ssl_file_ca", "connection_url", "local_id", "remote_id",
        "video_pref_size", "video_fps", "video_bandwidth_up_max", "video_bandwidth_down_max", "video_motion_rank", "video_congestion_ctrl_enabled",
        "video_jb_enabled", "video_zeroartifacts_enabled", "video_avpf_tail",
        "audio_echo_supp_enabled", "audio_echo_tail",
        "natt_ice_servers", "natt_ice_stun_enabled", "natt_ice_turn_enabled"
    };
    for (size_t i = 0; i < sizeof(__entries) / sizeof(__entries[0]); ++i) {
        CA_DEBUG_INFO_EX("CONFIG", "%s: %s", __entries[i], jsonConfig[__entries[i]].toStyledString().c_str());
    }
#else
    CA_DEBUG_INFO_EX(kCAMobuleNameTest, "Config = %s", jsonConfig.toStyledString().c_str());
#endif

    CA_ASSERT(CAEngine::setSSLCertificates(
                  jsonConfig["ssl_file_pub"].isString() ? jsonConfig["ssl_file_pub"].asCString() : "SSL_Pub.pem",
                  jsonConfig["ssl_file_priv"].isString() ? jsonConfig["ssl_file_priv"].asCString() : "SSL_Priv.pem",
                  jsonConfig["ssl_file_ca"].isString() ? jsonConfig["ssl_file_ca"].asCString() : "SSL_CA.pem"
              ));
    if (jsonConfig["video_pref_size"].isString()) {
        CA_ASSERT(CAEngine::setVideoPrefSize(jsonConfig["video_pref_size"].asCString()));
    }
    if (jsonConfig["video_fps"].isNumeric()) {
        CA_ASSERT(CAEngine::setVideoFps(jsonConfig["video_fps"].asInt()));
    }
    if (jsonConfig["video_bandwidth_up_max"].isNumeric()) {
        CA_ASSERT(CAEngine::setVideoBandwidthUpMax(jsonConfig["video_bandwidth_up_max"].asInt()));
    }
    if (jsonConfig["video_bandwidth_down_max"].isNumeric()) {
        CA_ASSERT(CAEngine::setVideoBandwidthDownMax(jsonConfig["video_bandwidth_down_max"].asInt()));
    }
    if (jsonConfig["video_motion_rank"].isNumeric()) {
        CA_ASSERT(CAEngine::setVideoMotionRank(jsonConfig["video_motion_rank"].asInt()));
    }
    if (jsonConfig["video_congestion_ctrl_enabled"].isBool()) {
        CA_ASSERT(CAEngine::setVideoCongestionCtrlEnabled(jsonConfig["video_congestion_ctrl_enabled"].asBool()));
    }
    if (jsonConfig["video_jb_enabled"].isBool()) {
        CA_ASSERT(CAEngine::setVideoJbEnabled(jsonConfig["video_jb_enabled"].asBool()));
    }
    if (jsonConfig["video_zeroartifacts_enabled"].isBool()) {
        CA_ASSERT(CAEngine::setVideoZeroArtifactsEnabled(jsonConfig["video_zeroartifacts_enabled"].asBool()));
    }
    if (jsonConfig["video_avpf_tail"].isString()) {
        char min[24], max[24];
        CA_ASSERT(sscanf(jsonConfig["video_avpf_tail"].asCString(), "%23s %23s", min, max) != EOF);
        CA_ASSERT(CAEngine::setVideoAvpfTail(atoi(min), atoi(max)));
    }
    if (jsonConfig["audio_echo_supp_enabled"].isBool()) {
        CA_ASSERT(CAEngine::setAudioEchoSuppEnabled(jsonConfig["audio_echo_supp_enabled"].asBool()));
    }
    if (jsonConfig["audio_echo_tail"].isNumeric()) {
        CA_ASSERT(CAEngine::setAudioEchoTail(jsonConfig["audio_echo_tail"].asInt()));
    }

    if (jsonConfig["natt_ice_servers"].isArray() && jsonConfig["natt_ice_servers"].size() > 0) {
        for (CAJson::ArrayIndex IceServerIndex = 0; IceServerIndex < jsonConfig["natt_ice_servers"].size(); ++IceServerIndex) {
            CA_ASSERT(CAEngine::addNattIceServer(
                          jsonConfig["natt_ice_servers"][IceServerIndex]["protocol"].asCString(),
                          jsonConfig["natt_ice_servers"][IceServerIndex]["host"].asCString(),
                          jsonConfig["natt_ice_servers"][IceServerIndex]["port"].asUInt(),
                          jsonConfig["natt_ice_servers"][IceServerIndex]["enable_turn"].asBool(),
                          jsonConfig["natt_ice_servers"][IceServerIndex]["enable_stun"].asBool(),
                          jsonConfig["natt_ice_servers"][IceServerIndex]["login"].asCString(),
                          jsonConfig["natt_ice_servers"][IceServerIndex]["password"].asCString()
                      ));
        }
    }

    if (jsonConfig["natt_ice_stun_enabled"].isBool()) {
        CA_ASSERT(CAEngine::setNattIceStunEnabled(jsonConfig["natt_ice_stun_enabled"].asBool()));
    }
    if (jsonConfig["natt_ice_turn_enabled"].isBool()) {
        CA_ASSERT(CAEngine::setNattIceTurnEnabled(jsonConfig["natt_ice_turn_enabled"].asBool()));
    }

    /* connect */
    signalSession = CASignaling::newObj(jsonConfig["connection_url"].asString(), jsonConfig["local_id"].asString(), jsonConfig["local_password"].asString());
    CA_ASSERT(signalSession);

    CA_ASSERT(signalSession->setCallback(CASignalingCallbackDummy::newObj()));
    CA_ASSERT(signalSession->connect());

    /* print help */
    printHelp();

#if CA_UNDER_WINDOWS
    mainThreadId = GetCurrentThreadId();
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) {
            break;
        }
        else if (msg.message == WM_CA_ATTACH_DISPLAYS) {
            CA_DEBUG_INFO("Catching 'WM_CA_ATTACH_DISPLAYS' windows event");
            CA_ASSERT(attachDisplays());
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    threadConsoleReader->join();
#endif

    threadConsoleReader = NULL;

    return 0;
}

static bool loadConfig()
{
    if (!CAUtils::fileExists(config_file_path)) {
        snprintf(config_file_path, sizeof(config_file_path), "%s/%s", CAUtils::currentDirectoryPath(), "config.json");
        if (!CAUtils::fileExists(config_file_path)) {
            static const char* config_file_paths[] = { "../../config.json", "../config.json" };
            for (size_t i = 0; i < sizeof(config_file_paths) / sizeof(config_file_paths[0]); ++i) {
                if (CAUtils::fileExists(config_file_paths[i])) {
                    memcpy(config_file_path, config_file_paths[i], strlen(config_file_paths[i]));
                    config_file_path[strlen(config_file_paths[i])] = '\0';
                    break;
                }
            }
        }
    }

    FILE* p_file = fopen(config_file_path, "rb");
    if (!p_file) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameTest, "Failed to open file at %s", config_file_path);
        return false;
    }
    fseek(p_file, 0, SEEK_END);
    long fsize = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);

    char *p_buffer = new char[fsize + 1];
    CA_ASSERT(p_buffer != NULL);
    p_buffer[fsize] = 0;
    CA_ASSERT(fread(p_buffer, 1, fsize, p_file) == fsize);
    std::string jsonText(p_buffer);
    fclose(p_file);
    delete[] p_buffer;

    CAJson::Reader reader;
    CA_ASSERT(reader.parse(jsonText, jsonConfig, false));

    return true;
}

static void printHelp()
{
    printf("----------------------------------------------------------------------------\n"
           "                               COMMANDS                                     \n"
           "----------------------------------------------------------------------------\n"
           "\"help\"               Prints this message\n"
           "\"chat [dest]\"        Sends random string to \"dest\" (opt., default from config.json)\n"
           "\"audio [dest]\"       Audio call to \"dest\" (opt., default from config.json)\n"
           "\"video [dest]\"       Video call to \"dest\" (opt., default from config.json)\n"
           "\"screencast [dest]\"  Share screen with \"dest\" (opt., default from config.json)\n"
           "\"mute\"               Mutes the active call\n"
           "\"unmute\"             Unmutes the active call\n"
           "\"hold\"               Holds the active call\n"
           "\"resume\"             Resumes the active call\n"
           "\"hangup\"             Terminates the active call\n"
           "\"accept\"             Accepts the incoming call\n"
           "\"reject\"             Rejects the incoming call\n"
		   "\"connect\"            Connects the signaling layer\n"
           "\"quit\"               Teminates the application\n"
           "--------------------------------------------------------------------------\n\n"
          );
}

static void* CA_STDCALL consoleReaderProc(void *arg)
{
    char command[1024] = { 0 };
    char remoteId[25];

    (remoteId);

    CA_DEBUG_INFO_EX(kCAMobuleNameTest, "consoleReaderProc \"ENTER\"");

    while (fgets(command, sizeof(command), stdin) != NULL) {
#define CHECK_CONNECTED() if (!connected){ CA_DEBUG_ERROR_EX(kCAMobuleNameTest, "+++ not connected yet +++"); continue; }
#define CHECK_NOT_CONNECTED() if (connected){ CA_DEBUG_ERROR_EX(kCAMobuleNameTest, "+++ already connected +++"); continue; }
        if (strnicmp(command, "quit", 4) == 0) {
            CA_DEBUG_INFO_EX(kCAMobuleNameTest, "+++ quit() +++");
            break;
        }
        else if (strnicmp(command, "help", 4) == 0) {
            CA_DEBUG_INFO_EX(kCAMobuleNameTest, "+++ help() +++");
            printHelp();
        }
		else if (strnicmp(command, "chat", 4) == 0) {
			std::string strRemoteId = jsonConfig["remote_id"].asString();
			std::string strIMContent = "Hello world :) <3";
			CHECK_CONNECTED();
			if (sscanf(command, "%*s %24s", remoteId) > 0 && strlen(remoteId) > 0) {
				strRemoteId = std::string(remoteId);
			}
			CAObjWrapper<CAResultTransac* >oResult;
			CA_ASSERT((oResult = signalSession->sendIM(strRemoteId, strIMContent.c_str(), strIMContent.length())));
			if (oResult->isOk()) {
				CA_DEBUG_INFO_EX(kCAMobuleNameTest, "Chat message sent: code = %u, callId = %s, transacId = %s", oResult->getCode(), oResult->getCallId().c_str(), oResult->getTransacId().c_str());
				chatMessages[oResult->getCallId()][oResult->getTransacId()] = oResult;
			}
			else {
				CA_DEBUG_ERROR_EX(kCAMobuleNameTest, "Failed to send chat message");
			}
		}
        else if (strnicmp(command, "audio", 5) == 0 || strnicmp(command, "video", 5) == 0 || strnicmp(command, "screencast", 10) == 0 || strnicmp(command, "call", 4) == 0) {
			std::string strRemoteId = jsonConfig["remote_id"].asString();
			CHECK_CONNECTED();
        }
        else if (strnicmp(command, "mute", 4) == 0 || strnicmp(command, "unmute", 6) == 0) {
			CHECK_CONNECTED();
        }
        else if (strnicmp(command, "hold", 4) == 0) {
			CHECK_CONNECTED();
        }
        else if (strnicmp(command, "resume", 6) == 0) {
			CHECK_CONNECTED();
        }
        else if (strnicmp(command, "hangup", 6) == 0 || strnicmp(command, "reject", 6) == 0) {
			CHECK_CONNECTED();
        }
        else if (strnicmp(command, "accept", 6) == 0) {
			CHECK_CONNECTED();
        }
		else if (strnicmp(command, "connect", 6) == 0) {
			CHECK_NOT_CONNECTED();
			CA_ASSERT(signalSession->connect());
		}		
    }

    CA_DEBUG_INFO_EX(kCAMobuleNameTest, "consoleReaderProc \"EXIT\"");

#if CA_UNDER_WINDOWS
    PostThreadMessage(mainThreadId, WM_QUIT, NULL, NULL);
#endif /* CA_UNDER_WINDOWS */

    return NULL;
}

static bool attachDisplays()
{
#if CA_UNDER_WINDOWS
    if ((mainThreadId != GetCurrentThreadId())) {
        CA_DEBUG_INFO_EX(kCAMobuleNameTest, "attachDisplays deferred because we are not on the main thread.... %lu<>%lu", mainThreadId, GetCurrentThreadId());
        PostThreadMessage(mainThreadId, WM_CA_ATTACH_DISPLAYS, NULL, NULL);
        return true;
    }
#endif /* CA_UNDER_WINDOWS */

    if (callSession) {
        if ((callSession->getMediaType() & CAMediaType_ScreenCast)) {
            CA_ASSERT(callSession->setVideoDisplays(CAMediaType_ScreenCast, getDisplay(false/*remote?*/, true/*screencast?*/), getDisplay(true/*remote?*/, true/*screencast?*/)));
        }
        if ((callSession->getMediaType() & CAMediaType_Video)) {
            CA_ASSERT(callSession->setVideoDisplays(CAMediaType_Video, getDisplay(false/*remote?*/, false/*screencast?*/), getDisplay(true/*remote?*/, false/*screencast?*/)));
        }
    }
    return true;
}

static CAVideoDisplay getDisplay(bool bRemote, bool bScreenCast /*= false*/)
{
#if CA_UNDER_WINDOWS
#if !defined(WS_OVERLAPPEDWINDOW)
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)
#endif /* WS_OVERLAPPEDWINDOW */
    HWND* pHWND = bRemote ? (bScreenCast ? &displayScreenCastRemote : &displayVideoRemote) : (bScreenCast ? &displayScreenCastLocal : &displayVideoLocal);
    if (!*pHWND) {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = L"MFCapture Window Class";
        /*GOTHAM_ASSERT*/(RegisterClass(&wc));

        CA_ASSERT((*pHWND = ::CreateWindow(
                                wc.lpszClassName,
                                bRemote ? DEFAULT_VIDEO_REMOTE_WINDOW_NAME : DEFAULT_VIDEO_LOCAL_WINDOW_NAME,
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                bRemote ? CW_USEDEFAULT : 352,
                                bRemote ? CW_USEDEFAULT : 288,
                                NULL,
                                NULL,
                                GetModuleHandle(NULL),
                                NULL)) != NULL);

        ::SetWindowText(*pHWND, bRemote ? DEFAULT_VIDEO_REMOTE_WINDOW_NAME : DEFAULT_VIDEO_LOCAL_WINDOW_NAME);
#if CA_UNDER_WINDOWS_CE
        ::ShowWindow(*pHWND, SW_SHOWNORMAL);
#else
        ::ShowWindow(*pHWND, SW_SHOWDEFAULT);
#endif /* CA_UNDER_WINDOWS_CE */

        ::UpdateWindow(*pHWND);
    }
    return *pHWND;
#else
    return NULL;
#endif /* CA_UNDER_WINDOWS */
}

#if CA_UNDER_WINDOWS

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
    case WM_CLOSE: {
        if (displayVideoLocal == hwnd) {
            displayVideoLocal = NULL;
        }
        else if (displayVideoRemote == hwnd) {
            displayVideoRemote = NULL;
        }
        else if (displayScreenCastLocal == hwnd) {
            displayScreenCastLocal = NULL;
        }
        else if (displayScreenCastRemote == hwnd) {
            displayScreenCastRemote = NULL;
        }
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif /* CA_UNDER_WINDOWS */
