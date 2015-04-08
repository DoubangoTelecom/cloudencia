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
#include "cloudencia/ca_msg.h"
#include "cloudencia/ca_result.h"
#include "cloudencia/ca_callbacks.h"

#include <string>

class CASignaling;
class CACallbackNet;

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
	friend class CASessionCall;
	friend class CACallbackNet;
protected:
	CASignaling(CAObjWrapper<CANetTransport* >& oNetTransport, CAObjWrapper<CAUrl* >& oConnectionUrl);
public:
	virtual ~CASignaling();
	virtual CA_INLINE const char* getObjectId() {
		return "CASignaling";
	}

	bool setCallbackNet(CAObjWrapper<CACallbackNet* > oCallback);
	bool setCallbackChat(CAObjWrapper<CACallbackChat* > oCallback);
	bool setCallbackAuthConn(CAObjWrapper<CACallbackAuthConn* > oCallback);
	bool isConnected();
	bool isReady();
	bool connect();
	bool disConnect();
	CAObjWrapper<CAResultTransac* > sendChatMessage(std::string strTo, const void* pcData, size_t nDataSize, std::string dataType = kContentTypeText);

	static CAObjWrapper<CASignaling* > newObj(std::string strConnectionUri, std::string strCredUserId, std::string strCredPassword, std::string pcLocalIP = "", unsigned short nLocalPort = 0);

	virtual CA_INLINE std::string getCredUserId() {
		return m_strCredUserId;
	}
	virtual CA_INLINE std::string getCredPassword() {
		return m_strCredPassword;
	}

private:
	bool sendData(const void* pcData, size_t nDataSize);
	bool handleData(const char* pcData, size_t nDataSize);
	bool canSendData();
	std::string randomString(std::string strPrefix);

	bool authConnection();

	void lock();
	void unlock();

private:
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
	CAObjWrapper<CANetTransport* > m_oNetTransport;
	CAObjWrapper<CAUrl* > m_oConnectionUrl;
	CAObjWrapper<CASignalingTransportCallback* > m_oNetCallback;
	CAObjWrapper<CAMsgAuthConn* > m_oMsgAuthConn;
	CAObjWrapper<CACallbackNet* > m_oCallbackNet;
	CAObjWrapper<CACallbackChat* > m_oCallbackChat;
	CAObjWrapper<CACallbackAuthConn* > m_oCallbackAuthConn;
	CANetFd m_Fd;
	bool m_bWsHandshakingDone;
	void* m_pWsSendBufPtr;
	bool m_bConnAuthenticated;
	tsk_size_t m_nWsSendBuffSize;
	CAObjWrapper<CAMutex* > m_oMutex;
	std::string m_strCredUserId;
	std::string m_strCredPassword;
	std::string m_strAuthToken;
	CA_DISABLE_WARNINGS_END()
};

#endif /* _CLOUDENCIA_SIGNALING_H_ */
