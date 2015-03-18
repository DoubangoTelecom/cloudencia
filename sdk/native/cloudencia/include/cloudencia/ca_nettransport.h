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
#if !defined(_CLOUDENCIA_NET_TRANSPORT_H_)
#define _CLOUDENCIA_NET_TRANSPORT_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_mutex.h"

#include <map>
#include <string>

class CANetTransport;
class CANetTransportCallback;

//
//	CANetPeer
//
class CANetPeer : public CAObj
{
	friend class CANetTransport;
	friend class CANetTransportCallback;
public:
	CANetPeer(CANetFd nFd, bool bConnected = false, const void* pcData = NULL, size_t nDataSize = 0);
	virtual ~CANetPeer();
	virtual CA_INLINE CANetFd getFd() {
		return  m_nFd;
	}
	virtual CA_INLINE bool isConnected() {
		return  m_bConnected;
	}
	virtual const void* getDataPtr();
	virtual size_t getDataSize();
	virtual CA_INLINE bool isRawContent() {
		return  m_bRawContent;
	}
	virtual CA_INLINE void setRawContent(bool bRawContent) {
		m_bRawContent = bRawContent;
	}
	virtual bool buildWsKey();
	virtual const char* getWsKey() {
		return m_pWsKey;
	}
	virtual CA_INLINE bool isStream() = 0;

protected:
	virtual CA_INLINE void setConnected(bool bConnected) {
		m_bConnected = bConnected;
	}

protected:
	bool m_bConnected;
	bool m_bRawContent;
	CANetFd m_nFd;
	struct tsk_buffer_s* m_pWrappedBuffer;
	char* m_pWsKey;
};


//
//	CANetPeerDgram
//
class CANetPeerDgram : public CANetPeer
{
public:
	CANetPeerDgram(CANetFd nFd, const void* pcData = NULL, size_t nDataSize = 0)
		:CANetPeer(nFd, false, pcData, nDataSize) {
	}
	virtual ~CANetPeerDgram() {
	}
	virtual CA_INLINE const char* getObjectId() {
		return "CANetPeerDgram";
	}
	virtual CA_INLINE bool isStream() {
		return false;
	}
};

//
//	CANetPeerStream
//
class CANetPeerStream : public CANetPeer
{
public:
	CANetPeerStream(CANetFd nFd, bool bConnected = false, const void* pcData = NULL, size_t nDataSize = 0)
		:CANetPeer(nFd, bConnected, pcData, nDataSize) {
	}
	virtual ~CANetPeerStream() {
	}
	virtual CA_INLINE const char* getObjectId() {
		return "CANetPeerStream";
	}
	virtual CA_INLINE bool isStream() {
		return true;
	}
	virtual bool appenData(const void* pcData, size_t nDataSize);
	virtual bool remoteData(size_t nPosition, size_t nSize);
	virtual bool cleanupData();
};

//
//	CANetTransport
//
class CANetTransportCallback : public CAObj
{
public:
	CANetTransportCallback() {
	}
	virtual ~CANetTransportCallback() {
	}
	virtual bool onData(CAObjWrapper<CANetPeer*> oPeer, size_t &nConsumedBytes) = 0;
	virtual bool onConnectionStateChanged(CAObjWrapper<CANetPeer*> oPeer) = 0;
};

//
//	CANetTransport
//
class CANetTransport : public CAObj
{
protected:
	CANetTransport(CANetTransporType_t eType, const char* pcLocalIP, unsigned short nLocalPort);
public:
	virtual ~CANetTransport();
	virtual CA_INLINE CANetTransporType_t getType() {
		return m_eType;
	}
	virtual CA_INLINE bool isStarted() {
		return m_bStarted;
	}
	virtual CA_INLINE bool isValid() {
		return m_bValid;
	}
	virtual CA_INLINE std::string getLocalIP() {
		return m_strLocalIP;
	}
	virtual CA_INLINE unsigned short getLocalPort() {
		return m_nLocalPort;
	}
	virtual CA_INLINE void setCallback(CAObjWrapper<CANetTransportCallback*> oCallback) {
		m_oCallback = oCallback;
	}

	virtual bool setSSLCertificates(const char* pcPrivateKey, const char* pcPublicKey, const char* pcCA, bool bVerify = false);
	virtual bool start();
	virtual CANetFd connectTo(const char* pcHost, unsigned short nPort);
	virtual bool isConnected(CANetFd nFd);
	virtual bool sendData(CANetFd nFdFrom, const void* pcDataPtr, size_t nDataSize);
	virtual bool sendData(CAObjWrapper<CANetPeer*> oPeer, const void* pcDataPtr, size_t nDataSize);
	virtual bool close(CANetFd nFd);
	virtual bool stop();

private:
	CAObjWrapper<CANetPeer*> getPeerByFd(CANetFd nFd);
	void insertPeer(CAObjWrapper<CANetPeer*> oPeer);
	void removePeer(CANetFd nFd);
	static int CANetTransportCb_Stream(const struct tnet_transport_event_s* e);

protected:
	CANativeNetTransportHandle_t* m_pWrappedTransport;
	CANetTransporType_t m_eType;
	bool m_bValid, m_bStarted;
	unsigned short m_nLocalPort;
	std::string m_strLocalIP;
	std::map<CANetFd, CAObjWrapper<CANetPeer*> > m_Peers;
	CAObjWrapper<CANetTransportCallback*> m_oCallback;
	CAObjWrapper<CAMutex*> m_oPeersMutex;
};


#endif /* _CLOUDENCIA_NET_TRANSPORT_H_ */
