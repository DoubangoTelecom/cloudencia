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
#if !defined(_CLOUDENCIA_SESSION_CALL_H_)
#define _CLOUDENCIA_SESSION_CALL_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_session.h"
#include "cloudencia/ca_signaling.h"
#include "cloudencia/ca_mutex.h"

#include <string>

typedef int CARoType; // mapped to "enum tmedia_ro_type_e"

class CASessionCall;

//
//	CASessionCallIceCallback
//
class CASessionCallIceCallback : public CAObj
{
public:
	CASessionCallIceCallback() {
	}
	virtual bool onStateChanged(CAObjWrapper<CASessionCall* > oCall) = 0;
};

class CASessionCall : public CASession
{
	friend class CAAutoLock<CASessionCall >;
protected:
	CASessionCall(CAObjWrapper<CASignaling*> oSignaling, std::string strCallId = "");
public:
	virtual ~CASessionCall();
	virtual CA_INLINE const char* getObjectId() {
		return "CASessionCall";
	}

	virtual bool setIceCallback(CAObjWrapper<CASessionCallIceCallback* > oIceCallback);
	
	virtual bool setVideoDisplays(CAMediaType_t eVideoType, int64_t displayLocal = 0, int64_t displayRemote = 0);
#if !defined(SWIG)
	virtual bool setVideoDisplays(CAMediaType_t eVideoType, CAVideoDisplay displayLocal = NULL, CAVideoDisplay displayRemote = NULL);
#endif

	virtual bool call(CAMediaType_t eMediaType, std::string strDestUserId);
	virtual bool acceptEvent(CAObjWrapper<CASignalingCallEvent* >& e);
	static bool rejectEvent(CAObjWrapper<CASignaling* > oSignaling, CAObjWrapper<CASignalingCallEvent* >& e);

	virtual bool setMute(bool bMuted, CAMediaType_t eMediaType = CAMediaType_All);
	virtual bool hangup();

	virtual bool setVideoFps(int nFps, CAMediaType_t eMediaType = CAMediaType_Video);
	virtual bool setVideoBandwidthUploadMax(int nMax, CAMediaType_t eMediaType = (CAMediaType_t)(CAMediaType_Video | CAMediaType_ScreenCast));
	virtual bool setVideoBandwidthDownloadMax(int nMax, CAMediaType_t eMediaType = (CAMediaType_t)(CAMediaType_Video | CAMediaType_ScreenCast));

	virtual CA_INLINE std::string getCallId() {
		return m_strCallId;    /**< Gets the call identifier */
	}
	virtual CA_INLINE CAMediaType_t getMediaType() {
		return m_eMediaType;    /**< Gets the active (negotiated) media type */
	}
	virtual CA_INLINE enum CAIceState_e getIceState() {
		return m_eIceState;
	}

	static CAObjWrapper<CASessionCall* > newObj(CAObjWrapper<CASignaling* > signalingSession);
	static CAObjWrapper<CASessionCall* > newObj(CAObjWrapper<CASignaling* > signalingSession, CAObjWrapper<CASignalingCallEvent* >& offer);

private:
	void lock();
	void unlock();

	bool cleanup();

	bool createSessionMgr();
	bool createLocalOffer(const struct tsdp_message_s* pc_Ro = NULL, CARoType eRoType = 0);
	bool attachVideoDisplays();

	struct tnet_ice_ctx_s* iceCreateCtx(bool bVideo);
	bool iceCreateCtxAll();
	bool iceSetTimeout(int32_t timeout);
	bool iceGotLocalCandidates(struct tnet_ice_ctx_s *p_IceCtx);
	bool iceGotLocalCandidatesAll();
	bool iceProcessRo(const struct tsdp_message_s* pc_SdpRo, bool isOffer);
	bool iceIsDone();
	bool iceIsEnabled(const struct tsdp_message_s* pc_Sdp);
	bool iceStart();
	static int iceCallback(const struct tnet_ice_event_s *e);

	bool sendSdp();

private:
	CAMediaType_t m_eMediaType;

	struct tnet_ice_ctx_s *m_pIceCtxVideo;
	struct tnet_ice_ctx_s *m_pIceCtxScreenCast;
	struct tnet_ice_ctx_s *m_pIceCtxAudio;

	struct tmedia_session_mgr_s* m_pSessionMgr;

	CAObjWrapper<CAMutex* > m_oMutex;

	CAObjWrapper<CASessionCallIceCallback* > m_oIceCallback;

	CAVideoDisplay m_VideoDisplayLocal;
	CAVideoDisplay m_VideoDisplayRemote;
	CAVideoDisplay m_ScreenCastDisplayLocal;
	CAVideoDisplay m_ScreenCastDisplayRemote;

	std::string m_strDestUserId;
	std::string m_strCallId;
	std::string m_strTidOffer;
	std::string m_strLocalSdpType;

	enum CAIceState_e m_eIceState;

	int m_nVideoBandwidthUploadMax;
	int m_nVideoBandwidthDownloadMax;
	int m_nVideoFps;
};

#endif /* _CLOUDENCIA_SESSION_CALL_H_ */
