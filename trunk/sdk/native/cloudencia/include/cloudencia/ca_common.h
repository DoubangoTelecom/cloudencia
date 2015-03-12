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
#if !defined(_CLOUDENCIA_COMMON_H_)
#define _CLOUDENCIA_COMMON_H_

#include "ca_config.h"

#define CA_CAT_(A, B) A ## B
#define CA_CAT(A, B) CA_CAT_(A, B)
#define CA_STRING_(A) #A
#define CA_STRING(A) CA_STRING_(A)

#if !defined(CA_SAFE_DELETE_CPP)
#	define CA_SAFE_DELETE_CPP(cpp_obj) if(cpp_obj) delete (cpp_obj), (cpp_obj) = NULL;
#endif

#define CA_ASSERT(x) { \
	bool __b_ret = (x); \
	assert(__b_ret); \
}

#define kJsonContentType "application/json"

#define kStunSoftware "Doubango WebRTC PaaS client"

#define kSchemeWSS "wss"
#define kSchemeWS "ws"

typedef int32_t CANetFd;
#define CANetFd_IsValid(self)	((self) > 0)
#define kCANetFdInvalid			-1

#if CA_UNDER_WINDOWS
#	define CAVideoDisplay HWND
#	if CA_UNDER_WINDOWS_CE
#		define CA_STDCALL __cdecl
#	else
#		define CA_STDCALL __stdcall
#	endif /* CA_UNDER_WINDOWS_CE */
#else
#	define CAVideoDisplay void*
#	define CA_STDCALL
#endif /* CA_UNDER_WINDOWS */

typedef enum CADebugLevel_e {
	CADebugLevel_Info = 4,
	CADebugLevel_Warn = 3,
	CADebugLevel_Error = 2,
	CADebugLevel_Fatal = 1
}
CADebugLevel_t;

typedef enum CASignalingEventType_e {
	CASignalingEventType_NetConnected,
	CASignalingEventType_NetReady,
	CASignalingEventType_NetData, // "passthrough" data: https://code.google.com/p/sincity/issues/detail?id=2
	CASignalingEventType_NetDisconnected,
	CASignalingEventType_NetError,

	CASignalingEventType_Call
}
CASignalingEventType_t;

#if 0
typedef enum CASessionState_e {
	CASessionState_None,
	CASessionState_Connecting,
	CASessionState_Connected,
	CASessionState_Terminated
}
CASessionState_t;
#endif

typedef enum CAMediaType_e {
	CAMediaType_None = 0x00,
	CAMediaType_Audio = (0x01 << 0),
	CAMediaType_Video = (0x01 << 1),
	CAMediaType_ScreenCast = (0x01 << 2),
	CAMediaType_AudioVideo = (CAMediaType_Audio | CAMediaType_Video),

	CAMediaType_All = 0xFF,
}
CAMediaType_t;

typedef enum CACallAction_e {
	CACallAction_None,
	CACallAction_Make,
	CACallAction_Accept
}
CACallAction_t;

typedef enum CACallState_e {
	CACallState_None,
	CACallState_IceGathering,
}
CACallState_t;

typedef enum CAIceState_e {
	CAIceState_None,
	CAIceState_Failed,
	CAIceState_GatheringDone,
	CAIceState_Connected,
	CAIceState_Teminated
}
CAIceState_t;

typedef enum CASessionType_e {
	CASessionType_None,
	CASessionType_Call
}
CASessionType_t;

typedef enum CANetTransporType_e {
	CANetTransporType_None = 0x00,
	CANetTransporType_TCP = (0x01 << 0),
	CANetTransporType_TLS = (0x01 << 1),
	CANetTransporType_WS = ((0x01 << 2) | CANetTransporType_TCP),
	CANetTransporType_WSS = ((0x01 << 3) | CANetTransporType_TLS),
	CANetTransporType_HTTP = ((0x01 << 4) | CANetTransporType_TCP),
	CANetTransporType_HTTPS = ((0x01 << 5) | CANetTransporType_TLS),
}
CANetTransporType_t;

typedef enum CAWsActionType_e {
	CAWsActionType_None = 0,
}
CAWsActionType_t;

typedef enum CAUrlHostType_e {
	CAUrlHostType_None = 0,
	CAUrlHostType_Hostname,
	CAUrlHostType_IPv4,
	CAUrlHostType_IPv6
}
CAUrlHostType_t;

typedef enum CAUrlType_e {
	CAUrlType_None = CANetTransporType_None,
	CAUrlType_TCP = CANetTransporType_TCP,
	CAUrlType_TLS = CANetTransporType_TLS,
	CAUrlType_WS = CANetTransporType_WS,
	CAUrlType_WSS = CANetTransporType_WSS,
	CAUrlType_HTTP = CANetTransporType_HTTP,
	CAUrlType_HTTPS = CANetTransporType_HTTPS,
}
CAUrlType_t;

static bool CA_INLINE CANetTransporType_isStream(CANetTransporType_t eType)
{
	switch (eType) {
	case CANetTransporType_TCP:
	case CANetTransporType_TLS:
	case CANetTransporType_WS:
	case CANetTransporType_WSS:
		return true;
	default:
		return false;
	}
}

#define kCAMobuleNameNetTransport "NetTransport"
#define kCAMobuleNameWsTransport "WebSocketTransport"
#define kCAMobuleNameSignaling "Signaling"
#define kCAMobuleNameFakeDisplay "Fake display"
#define kCAMobuleNameWEC7 "WEC7"


template <typename T>
class CAAutoLock
{
public:
	explicit CAAutoLock(T* obj) : obj_(obj) {
		obj_->lock();
	}
	virtual ~CAAutoLock() {
		obj_->unlock();
	}
protected:
	T* obj_;
};

typedef void CANativeMutexHandle_t; // Mapping to "tsk_mutex_handle_t"
typedef void CANativeNetTransportHandle_t; // Mapping to "tnet_transport_handle_t"
typedef void CANativeThreadHandle_t; // Mapping to "tsk_thread_handle_t"

#endif /* _CLOUDENCIA_COMMON_H_ */
