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
#if !defined(_CLOUDENCIA_SIGNALING_H_)
#define _CLOUDENCIA_SIGNALING_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_nettransport.h"
#include "cloudencia/ca_url.h"
#include "cloudencia/ca_mutex.h"

#include <string>

class CASignaling;

/**@ingroup _Group_CPP_Signaling
* Signaling event.
*/
class CLOUDENCIA_API CASignalingEvent : public CAObj
{
	friend class CASignaling;
public:
	CASignalingEvent(CASignalingEventType_t eType, std::string strDescription, const void* pcDataPtr = NULL, size_t nDataSize = 0);
	virtual ~CASignalingEvent();
	virtual CA_INLINE const char* getObjectId() {
		return "CASignalingEvent";
	}

	/**< The event type */
	virtual CA_INLINE CASignalingEventType_t getType()const {
		return m_eType;
	}
	/**< The event description */
	virtual CA_INLINE std::string getDescription()const {
		return m_strDescription;
	}
	/**< The event data pointer */
	virtual CA_INLINE const void* getDataPtr()const {
		return m_pDataPtr;
	}
	virtual CA_INLINE size_t getDataSize()const {
		return m_nDataSize;
	}

private:
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
	CASignalingEventType_t m_eType;
	std::string m_strDescription;
	void* m_pDataPtr;
	size_t m_nDataSize;
	CA_DISABLE_WARNINGS_END()
};


/**@ingroup _Group_CPP_Signaling
* Signaling event for call sessions.
*/
class CLOUDENCIA_API CASignalingCallEvent : public CASignalingEvent
{
	friend class CASignaling;
public:
	CASignalingCallEvent(std::string strDescription);
	virtual ~CASignalingCallEvent();
	virtual CA_INLINE const char* getObjectId() {
		return "CASignalingCallEvent";
	}

	/**< The event type. e.g. "offer", "answer", "hangup"... */
	CA_INLINE std::string getType() {
		return m_strType;
	}
	/**< The source identifier */
	CA_INLINE std::string getFrom() {
		return m_strFrom;
	}
	/**< The destination identifier */
	CA_INLINE std::string getTo() {
		return m_strTo;
	}
	/**< The call identifier */
	CA_INLINE std::string getCallId() {
		return m_strCallId;
	}
	/**< The transaction identifier */
	CA_INLINE std::string getTransacId() {
		return m_strTransacId;
	}
	/**< The session description. Could be NULL. */
	CA_INLINE std::string getSdp() {
		return m_strSdp;
	}

private:
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
	std::string m_strFrom;
	std::string m_strTo;
	std::string m_strSdp;
	std::string m_strType;
	std::string m_strCallId;
	std::string m_strTransacId;
	CA_DISABLE_WARNINGS_END()
};

/**@ingroup _Group_CPP_Signaling
* Callback class for the signaling session. You must override this call.
*/
class CLOUDENCIA_API CASignalingCallback : public CAObj
{
protected:
	CASignalingCallback() {}
public:
	virtual ~CASignalingCallback() {}
	/** Raised to signal events releated to the network connection states */
	virtual bool onEventNet(CAObjWrapper<CASignalingEvent* >& e) = 0;
	/** Raised to signal events related to the call states */
	virtual bool onEventCall(CAObjWrapper<CASignalingCallEvent* >& e) = 0;
};


/* Network transport used by the signaling layer.
*/
#if !defined(SWIG)
class CASignalingTransportCallback : public CANetTransportCallback
{
public:
	CASignalingTransportCallback(const CASignaling* pcCASignaling);
	virtual ~CASignalingTransportCallback();
	virtual CA_INLINE const char* getObjectId() {
		return "CASignalingTransportCallback";
	}

	virtual bool onData(CAObjWrapper<CANetPeer* > oPeer, size_t &nConsumedBytes);

	virtual bool onConnectionStateChanged(CAObjWrapper<CANetPeer* > oPeer);

private:
	const CASignaling* m_pcCASignaling;
};
#endif /* !defined(SWIG) */

class CLOUDENCIA_API CASignaling : public CAObj
{
	friend class CASignalingTransportCallback;
	friend class CAAutoLock<CASignaling >;
protected:
	CASignaling(CAObjWrapper<CANetTransport* >& oNetTransport, CAObjWrapper<CAUrl* >& oConnectionUrl);
public:
	virtual ~CASignaling();
	virtual CA_INLINE const char* getObjectId() {
		return "CASignaling";
	}

	bool setCallback(CAObjWrapper<CASignalingCallback* > oCallback);
	bool isConnected();
	bool isReady();
	bool connect();
	bool sendData(const void* pcData, size_t nDataSize);
	bool disConnect();

	static CAObjWrapper<CASignaling* > newObj(std::string strConnectionUri, std::string strCredUserId, std::string strCredPassword, std::string pcLocalIP = "", unsigned short nLocalPort = 0);

	virtual CA_INLINE std::string getCredUserId() {
		return m_strCredUserId;
	}
	virtual CA_INLINE std::string getCredPassword() {
		return m_strCredPassword;
	}

private:
	bool handleData(const char* pcData, size_t nDataSize);
	bool raiseEvent(CASignalingEventType_t eType, std::string strDescription, const void* pcDataPtr = NULL, size_t nDataSize = 0);

	void lock();
	void unlock();

private:
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
	CAObjWrapper<CANetTransport* > m_oNetTransport;
	CAObjWrapper<CAUrl* > m_oConnectionUrl;
	CAObjWrapper<CASignalingTransportCallback* > m_oNetCallback;
	CAObjWrapper<CASignalingCallback* > m_oSignCallback;
	CANetFd m_Fd;
	bool m_bWsHandshakingDone;
	void* m_pWsSendBufPtr;
	tsk_size_t m_nWsSendBuffSize;
	CAObjWrapper<CAMutex* > m_oMutex;
	std::string m_strCredUserId;
	std::string m_strCredPassword;
	CA_DISABLE_WARNINGS_END()
};

#endif /* _CLOUDENCIA_SIGNALING_H_ */
