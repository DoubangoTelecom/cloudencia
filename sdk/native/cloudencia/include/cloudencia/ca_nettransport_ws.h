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
#if !defined(_CLOUDENCIA_NET_TRANSPORT_WS_H_)
#define _CLOUDENCIA_NET_TRANSPORT_WS_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_url.h"
#include "cloudencia/ca_nettransport.h"

class CAWsTransport;

//
//	CAWsResult
//
class CAWsResult : public CAObj
{
public:
	CAWsResult(unsigned short nCode, const char* pcPhrase, const void* pcDataPtr = NULL, size_t nDataSize = 0, CAWsActionType_t eActionType = CAWsActionType_None);
	virtual ~CAWsResult();
	virtual CA_INLINE const char* getObjectId() {
		return "CAWsResult";
	}

	virtual CA_INLINE unsigned short getCode() {
		return m_nCode;
	}
	virtual CA_INLINE const char* getPhrase() {
		return m_pPhrase;
	}
	virtual CA_INLINE const void* getDataPtr() {
		return m_pDataPtr;
	}
	virtual CA_INLINE size_t getDataSize() {
		return m_nDataSize;
	}
	virtual CA_INLINE CAWsActionType_t getActionType() {
		return m_eActionType;
	}

private:
	unsigned short m_nCode;
	char* m_pPhrase;
	void* m_pDataPtr;
	size_t m_nDataSize;
	CAWsActionType_t m_eActionType;
};

//
//	CAWsTransportCallback
//
class CAWsTransportCallback : public CANetTransportCallback
{
public:
	CAWsTransportCallback(const CAWsTransport* pcTransport);
	virtual ~CAWsTransportCallback();
	virtual CA_INLINE const char* getObjectId() {
		return "CAWsTransportCallback";
	}
	virtual bool onData(CAObjWrapper<CANetPeer*> oPeer, size_t &nConsumedBytes);
	virtual bool onConnectionStateChanged(CAObjWrapper<CANetPeer*> oPeer);
private:
	const CAWsTransport* m_pcTransport;
};


//
//	CAWsTransport
//
class CAWsTransport : public CANetTransport
{
	friend class CAWsTransportCallback;
public:
	CAWsTransport(bool isSecure, const char* pcLocalIP = NULL, unsigned short nLocalPort = 0);
	virtual ~CAWsTransport();
	virtual CA_INLINE const char* getObjectId() {
		return "CAWsTransport";
	}

	bool handshaking(CAObjWrapper<CANetPeer*> oPeer, CAObjWrapper<CAUrl*> oUrl);

private:
	CAObjWrapper<CAWsResult*> handleJsonContent(CAObjWrapper<CANetPeer*> oPeer, const void* pcDataPtr, size_t nDataSize)const;
	bool sendResult(CAObjWrapper<CANetPeer*> oPeer, CAObjWrapper<CAWsResult*> oResult)const;

private:
	CAObjWrapper<CAWsTransportCallback*> m_oCallback;
};


#endif /* _CLOUDENCIA_NET_TRANSPORT_WS_H_ */
