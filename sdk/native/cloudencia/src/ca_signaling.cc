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
#include "cloudencia/ca_signaling.h"
#include "cloudencia/ca_nettransport_ws.h"
#include "cloudencia/ca_engine.h"
#include "cloudencia/ca_utils.h"
#include "cloudencia/ca_parser_url.h"
#include "cloudencia/jsoncpp/ca_json.h"

#include "tinyhttp.h"
#include "tsk_string.h"

#include <assert.h>

/**@defgroup _Group_CPP_Signaling Signaling
* @brief Signaling session class.
*/

#define CA_JSON_GET(fieldParent, fieldVarName, fieldName, typeTestFun, couldBeNull) \
	if(!fieldParent.isObject()) \
		{ \
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "JSON '%s' not an object", (fieldName)); \
		return false; \
		} \
	const CAJson::Value fieldVarName = (fieldParent)[(fieldName)]; \
	if((fieldVarName).isNull()) \
		{ \
		if(!(couldBeNull)) \
				{ \
			CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "JSON '%s' is null", (fieldName)); \
			return false; \
				}\
		} \
		else if(!(fieldVarName).typeTestFun()) \
		{ \
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "JSON '%s' has invalid type", (fieldName)); \
		return false; \
		}

/*
* Protected constructor to create new signaling session.
*/
CASignaling::CASignaling(CAObjWrapper<CANetTransport*>& oNetTransport, CAObjWrapper<CAUrl*>& oConnectionUrl)
    : m_oNetTransport(oNetTransport)
    , m_oConnectionUrl(oConnectionUrl)
    , m_Fd(kCANetFdInvalid)
    , m_bWsHandshakingDone(false)
    , m_bConnAuthenticated(false)
    , m_pWsSendBufPtr(NULL)
    , m_nWsSendBuffSize(0)
    , m_oMutex(new CAMutex())
{
    CA_ASSERT(m_oNetTransport);
    CA_ASSERT(m_oConnectionUrl);

    m_oNetCallback = new CASignalingTransportCallback(this);
    m_oNetTransport->setCallback(*m_oNetCallback);
}

/*
* Signaling session destructor.
*/
CASignaling::~CASignaling()
{
    m_oNetTransport->setCallback(NULL);
    TSK_FREE(m_pWsSendBufPtr);
}

/*
* Locks the signaling session object to avoid conccurent access. The mutex is recurssive.
*/
void CASignaling::lock()
{
    m_oMutex->lock();
}

/*
* Unlocks the signaling session object. The mutex is recurssive.
*/
void CASignaling::unlock()
{
    m_oMutex->unlock();
}

/**@ingroup _Group_CPP_Signaling
* Sets the callback object.
* @param callback Callback object.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CASignaling::setCallback(CAObjWrapper<CASignalingCallback*> callback)
{
    CAAutoLock<CASignaling> autoLock(this);

    m_oSignCallback = callback;
    return true;
}

/**@ingroup _Group_CPP_Signaling
* Checks whether the signaling session is connected to the network. Being connected doesn't mean the handshaking is done.
* Use @ref isReady() to check if the session is ready to send/receive data.
* @retval <b>true</b> if connected; otherwise <b>false</b>.
* @sa @ref isReady()
*/
bool CASignaling::isConnected()
{
    CAAutoLock<CASignaling> autoLock(this);

    return m_oNetTransport->isConnected(m_Fd);
}

/**@ingroup _Group_CPP_Signaling
* Checks whether the signaling session is ready to send/receive data. A signaling session using WebSocket transport will be ready when the handshaking is done.
* @retval <b>true</b> if ready; otherwise <b>false</b>.
* @sa @ref isConnected()
*/
bool CASignaling::isReady()
{
    CAAutoLock<CASignaling> autoLock(this);

    if (!isConnected()) {
        return false;
    }
    if (m_oConnectionUrl->getType() == CAUrlType_WS || m_oConnectionUrl->getType() == CAUrlType_WSS) {
        return m_bWsHandshakingDone;
    }
    return true;
}

/**@ingroup _Group_CPP_Signaling
* Connects the signaling session to the remove server. The connection will be done asynchronously and you need to use @ref setCallback() to register a new callback
* object to listen for network events. When WebSocket transport is used this function will start the handshaking phase once the TCP/TLS socket is connected.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
* @sa @ref disConnect()
*/
bool CASignaling::connect()
{
    CAAutoLock<CASignaling> autoLock(this);

    if (isConnected()) {
        CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "Already connected");
        return true;
    }

    if (!m_oNetTransport->isStarted()) {
        if (!m_oNetTransport->start()) {
            return false;
        }
    }

    CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "Connecting to [%s:%u]", m_oConnectionUrl->getHost().c_str(), m_oConnectionUrl->getPort());
    m_Fd = m_oNetTransport->connectTo(m_oConnectionUrl->getHost().c_str(), m_oConnectionUrl->getPort());
    return CANetFd_IsValid(m_Fd);
}

/**@ingroup _Group_CPP_Signaling
* Disconnect the signaling session.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
* @sa @ref connect()
*/
bool CASignaling::disConnect()
{
    CAAutoLock<CASignaling> autoLock(this);

    if (isConnected()) {
        bool ret = m_oNetTransport->close(m_Fd);
        if (ret) {
            // m_Fd = kCANetFdInvalid;
        }
        return ret;
    }
    return true;
}

/**@ingroup _Group_CPP_Signaling
* Sends Instant Message (IM).
* @param strTo
* @param pcData
* @param nDataSize
* @param dataType
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CASignaling::sendIM(std::string strTo, const void* pcData, size_t nDataSize, std::string dataType /*= kContentTypeText*/)
{
	CAAutoLock<CASignaling> autoLock(this);
	
	if (strTo.empty() || !pcData || !nDataSize) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Invalid argument");
		return false;
	}
	if (!isReady()) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Not ready yet");
		return false;
	}
	CAObjWrapper<CAMsgChat* > oMsg = new CAMsgChat(
		m_strCredUserId,
		m_strAuthToken,
		randomString(),
		randomString(),
		strTo,
		dataType,
		pcData,
		nDataSize);
	CA_ASSERT(oMsg);
	std::string jsonContent = oMsg->toJson();
	CA_ASSERT(!jsonContent.empty());
	return sendData(jsonContent.c_str(), jsonContent.length());
}

/**@ingroup _Group_CPP_Signaling
* Creates new signaling session object.
* @param pcConnectionUri A valid request URI (e.g. <b>ws://localhost:9000/wsStringStaticMulti?roomId=0</b>).
* @param strCredUserId The user login (email address). This is required and must not be empty.
* @param strCredPassword The user password (4 digit pin code). This is required and must not bu empty.
* @param pcLocalIP Local IP address to bind to. Best one will be used if not defined.
* @param nLocalPort Local Port to bind to. Best one will be used if not defined.
* @retval <b>newobject</b> if no error; otherwise <b>NULL</b>.
*/
CAObjWrapper<CASignaling* > CASignaling::newObj(std::string pcConnectionUri, std::string strCredUserId, std::string strCredPassword, std::string pcLocalIP /*=""*/, unsigned short nLocalPort /*= 0*/)
{
    CAObjWrapper<CASignaling*> oSignaling;
    CAObjWrapper<CAUrl*> oUrl;
    CAObjWrapper<CANetTransport*> oTransport;

    if (pcConnectionUri.empty()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "RequestUri is null or empty");
        goto bail;
    }
    if (strCredUserId.empty() || strCredPassword.empty()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "User credentials (id and password) are required");
        goto bail;
    }

    if (!CAEngine::isInitialized()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Engine not initialized");
        goto bail;
    }

    oUrl = ca_url_parse(pcConnectionUri.c_str(), pcConnectionUri.length());
    if (!oUrl) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Failed to parse request Uri: %s", pcConnectionUri);
        goto bail;
    }
    if (oUrl->getHostType() == CAUrlHostType_None) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Invalid host type: %s // %d", pcConnectionUri, oUrl->getHostType());
        goto bail;
    }
    switch (oUrl->getType()) {
    case CAUrlType_WS:
    case CAUrlType_WSS: {
        oTransport = new CAWsTransport((oUrl->getType() == CAUrlType_WSS), pcLocalIP.empty() ? NULL : pcLocalIP.c_str(), nLocalPort);
        if (!oTransport) {
            goto bail;
        }
        break;
    }
    default: {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Url type=%d not supported yet. Url=%s", oUrl->getType(), pcConnectionUri);
        goto bail;
    }
    }

    oSignaling = new CASignaling(oTransport, oUrl);
    oSignaling->m_strCredUserId = strCredUserId;
    oSignaling->m_strCredPassword = strCredPassword;
    oSignaling->m_strAuthToken = CAUtils::buildAuthToken(strCredUserId, strCredPassword);

bail:
    return oSignaling;
}

/*
* Sends data to the server.
* @param _pcData Pointer to the data to send.
* @param _nDataSize Size (in bytes) of the data to send.
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CASignaling::sendData(const void* _pcData, tsk_size_t _nDataSize)
{
	CAAutoLock<CASignaling> autoLock(this);

	if (!_pcData || !_nDataSize) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Invalid argument");
		return false;
	}

	if (!isReady()) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Not ready yet");
		return false;
	}

	CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "Send DATA:%.*s", _nDataSize, _pcData);

	if (m_oConnectionUrl->getType() == CAUrlType_WS || m_oConnectionUrl->getType() == CAUrlType_WSS) {
		if (!m_bWsHandshakingDone) {
			CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "WebSocket handshaking not done yet");
			return false;
		}
		uint8_t mask_key[4] = { 0x00, 0x00, 0x00, 0x00 };
		const uint8_t* pcData = (const uint8_t*)_pcData;
		uint64_t nDataSize = 1 + 1 + 4/*mask*/ + _nDataSize;
		uint64_t lsize = (uint64_t)_nDataSize;
		uint8_t* pws_snd_buffer;

		if (lsize > 0x7D && lsize <= 0xFFFF) {
			nDataSize += 2;
		}
		else if (lsize > 0xFFFF) {
			nDataSize += 8;
		}
		if (m_nWsSendBuffSize < nDataSize) {
			if (!(m_pWsSendBufPtr = tsk_realloc(m_pWsSendBufPtr, (tsk_size_t)nDataSize))) {
				CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Failed to allocate buffer with size = %llu", nDataSize);
				m_nWsSendBuffSize = 0;
				return 0;
			}
			m_nWsSendBuffSize = (tsk_size_t)nDataSize;
		}
		pws_snd_buffer = (uint8_t*)m_pWsSendBufPtr;

		pws_snd_buffer[0] = 0x81; // FIN | opcode-non-control::text
		pws_snd_buffer[1] = 0x80; // Set Mask flag (required for data from client->sever)

		if (lsize <= 0x7D) {
			pws_snd_buffer[1] |= (uint8_t)lsize;
			pws_snd_buffer = &pws_snd_buffer[2];
		}
		else if (lsize <= 0xFFFF) {
			pws_snd_buffer[1] |= 0x7E;
			pws_snd_buffer[2] = (lsize >> 8) & 0xFF;
			pws_snd_buffer[3] = (lsize & 0xFF);
			pws_snd_buffer = &pws_snd_buffer[4];
		}
		else {
			pws_snd_buffer[1] |= 0x7F;
			pws_snd_buffer[2] = (lsize >> 56) & 0xFF;
			pws_snd_buffer[3] = (lsize >> 48) & 0xFF;
			pws_snd_buffer[4] = (lsize >> 40) & 0xFF;
			pws_snd_buffer[5] = (lsize >> 32) & 0xFF;
			pws_snd_buffer[6] = (lsize >> 24) & 0xFF;
			pws_snd_buffer[7] = (lsize >> 16) & 0xFF;
			pws_snd_buffer[8] = (lsize >> 8) & 0xFF;
			pws_snd_buffer[9] = (lsize & 0xFF);
			pws_snd_buffer = &pws_snd_buffer[10];
		}

		// Mask Key
		pws_snd_buffer[0] = mask_key[0];
		pws_snd_buffer[1] = mask_key[1];
		pws_snd_buffer[2] = mask_key[2];
		pws_snd_buffer[3] = mask_key[3];
		pws_snd_buffer = &pws_snd_buffer[4];
		// Mask dat
		// ... nothing done because key is always zeros

		// append payload to headers
		memcpy(pws_snd_buffer, pcData, (size_t)lsize);
		// send() data
		return m_oNetTransport->sendData(m_Fd, m_pWsSendBufPtr, (tsk_size_t)nDataSize);
	}
	else {
		CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Not implemented yet");
		return false;
	}
}

/*
* Process incoming data
* @param pcData
* @param nDataSize
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CASignaling::handleData(const char* pcData, tsk_size_t nDataSize)
{
    CAAutoLock<CASignaling> autoLock(this);

    if (!pcData || !nDataSize) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Invalid parameter");
        return false;
    }

	CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "handleData(%.*s)", nDataSize, pcData);

    CAObjWrapper<CAMsg* >oMsg = CAMsg::parse(std::string((const char*)pcData, nDataSize));
    if (!oMsg) {
        return false;
    }

    if (oMsg->getType() == CAMsgType_Error) {
        CAObjWrapper<CAMsgError* >oMsgError = dynamic_cast<CAMsgError* >(*oMsg);
        CA_ASSERT(oMsgError);
        return raiseEvent(CASignalingEventType_Error, oMsgError->getReason());
    }
	else if (oMsg->getType() == CAMsgType_Provisional) {
		CAObjWrapper<CAMsgProvisional* >oMsgProvisional = dynamic_cast<CAMsgProvisional* >(*oMsg);
		CA_ASSERT(oMsgProvisional);
		
	}
	else if (oMsg->getType() == CAMsgType_Success) {
		CAObjWrapper<CAMsgSuccess* >oMsgSuccess = dynamic_cast<CAMsgSuccess* >(*oMsg);
		CA_ASSERT(oMsgSuccess);
		
		if (oMsgSuccess->isFor(*m_oMsgAuthConn)) {
			if (!m_bConnAuthenticated) {
				m_bConnAuthenticated = true;
				raiseEvent(CASignalingEventType_NetReady, "Ready");
			}
		}
	}
    else if (oMsg->getType() == CAMsgType_AuthConn) {
    }

#if 0 // FIXME
    CAJson::Value root;
    CAJson::Reader reader;

    // Parse JSON content
    bool parsingSuccessful = reader.parse((const char*)pcData, (((const char*)pcData) + nDataSize), root);
    if (!parsingSuccessful) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Failed to parse JSON content: %.*s", nDataSize, pcData);
        return false;
    }

    CA_JSON_GET(root, passthrough, "passthrough", isBool, true);

    if (passthrough.isBool() && passthrough.asBool() == true) {
        if (m_oSignCallback) {
            return raiseEvent(CASignalingEventType_NetData, "'passthrough' JSON data", (const void*)pcData, nDataSize);
        }
        return false;
    }

    CA_JSON_GET(root, type, "type", isString, false);
    CA_JSON_GET(root, cid, "cid", isString, false);
    CA_JSON_GET(root, tid, "tid", isString, false);
    CA_JSON_GET(root, from, "from", isString, false);
    CA_JSON_GET(root, to, "to", isString, false);

    if (to.asString() != CAEngine::s_strCredUserId) {
        if (from.asString() == CAEngine::s_strCredUserId) {
            CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "Ignoring loopback message with type='%s', call-id='%s', to='%s'", type.asCString(), cid.asCString(), to.asCString());
        }
        else {
            CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "Failed to match destination id: '%s'<>'%s'", to.asCString(), CAEngine::s_strCredUserId.c_str());
        }
        return false;
    }

    bool bIsCallEventRequiringSdp = (type.asString().compare("offer") == 0 || type.asString().compare("answer") == 0 || type.asString().compare("pranswer") == 0);
    if (bIsCallEventRequiringSdp || type.asString().compare("hangup") == 0) {
        if (m_oSignCallback) {
            CAObjWrapper<CASignalingCallEvent*> oCallEvent = new CASignalingCallEvent(type.asString());
            if (bIsCallEventRequiringSdp) {
                CA_JSON_GET(root, sdp, "sdp", isString, false);
                oCallEvent->m_strSdp = sdp.asString();
            }
            oCallEvent->m_strType = type.asString();
            oCallEvent->m_strCallId = cid.asString();
            oCallEvent->m_strTransacId = tid.asString();
            oCallEvent->m_strFrom = from.asString();
            oCallEvent->m_strTo = to.asString();
            return m_oSignCallback->onEventCall(oCallEvent);
        }
    }
    else {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Message with type='%s' not supported yet", type.asString().c_str());
        return false;
    }
#endif

    return true;
}

/*
* Raises an event.
* @param eType
* @param strDescription
* @retval <b>true</b> if no error; otherwise <b>false</b>.
*/
bool CASignaling::raiseEvent(CASignalingEventType_t eType, std::string strDescription, const void* pcDataPtr /*= NULL*/, size_t nDataSize /*= 0*/)
{
    CAAutoLock<CASignaling> autoLock(this);

    if (m_oSignCallback) {
        CAObjWrapper<CASignalingEvent*> e = new CASignalingEvent(eType, strDescription, pcDataPtr, nDataSize);
        return m_oSignCallback->onEventNet(e);
    }
    return true;
}

/*
Checks we can send data.
*/
bool CASignaling::canSendData()
{
    CAAutoLock<CASignaling> autoLock(this);

    if (!isConnected()) {
        return false;
    }
    if ((m_oConnectionUrl->getType() == CAUrlType_WS || m_oConnectionUrl->getType() == CAUrlType_WSS)) {
        return m_bWsHandshakingDone;
    }
    return true;
}

/*
Generates random string.
*/
std::string CASignaling::randomString()
{
    return CAUtils::randomString(m_oNetTransport->getLocalIP(), "@", CAUtils::itoa(m_oNetTransport->getLocalPort()), "@");
}

/*
Authenticate the connection. Must be established
*/
bool CASignaling::authConnection()
{
    CAAutoLock<CASignaling> autoLock(this);

    if (m_bConnAuthenticated) {
        return true;
    }
    if (!canSendData()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Not ready to send data");
        return false;
    }

    m_oMsgAuthConn = new CAMsgAuthConn(
        m_strCredUserId, // from
        m_strAuthToken, // authToken
        randomString(), // callId
        randomString() // transactionId
    );
    CA_ASSERT(m_oMsgAuthConn);
    std::string jsonContent = m_oMsgAuthConn->toJson();
    CA_ASSERT(!jsonContent.empty());
    return sendData(jsonContent.c_str(), jsonContent.length());
}

//
//	CASignalingTransportCallback
//

CASignalingTransportCallback::CASignalingTransportCallback(const CASignaling* pcCASignaling)
    : m_pcCASignaling(pcCASignaling)
{

}

CASignalingTransportCallback::~CASignalingTransportCallback()
{
    m_pcCASignaling = NULL;
}

bool CASignalingTransportCallback::onData(CAObjWrapper<CANetPeer*> oPeer, size_t &nConsumedBytes)
{
    CAAutoLock<CASignaling> autoLock(const_cast<CASignaling*>(m_pcCASignaling));

    CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "Incoming data = %.*s", oPeer->getDataSize(), (const char*)oPeer->getDataPtr());

    if (m_pcCASignaling->m_oConnectionUrl->getType() != CAUrlType_WS && m_pcCASignaling->m_oConnectionUrl->getType() != CAUrlType_WSS) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Not implemented yet");
        return false;
    }

    if (!m_pcCASignaling->m_bWsHandshakingDone) {
        /* WebSocket handshaking data */

        if (oPeer->getDataSize() > 4) {
            const char* pcData = (const char*)oPeer->getDataPtr();
            if (pcData[0] == 'H' && pcData[1] == 'T' && pcData[2] == 'T' && pcData[3] == 'P') {
                int endOfMessage = tsk_strindexOf(pcData, oPeer->getDataSize(), "\r\n\r\n"/*2CRLF*/) + 4;
                if (endOfMessage > 4) {
                    thttp_message_t *p_msg = tsk_null;
                    const thttp_header_Sec_WebSocket_Accept_t* http_hdr_accept;
                    tsk_ragel_state_t ragel_state;

                    tsk_ragel_state_init(&ragel_state, pcData, (tsk_size_t)endOfMessage);
                    if (thttp_message_parse(&ragel_state, &p_msg, tsk_false) != 0) {
                        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Failed to parse HTTP message: %.*s", endOfMessage, pcData);
                        TSK_OBJECT_SAFE_FREE(p_msg);
                        return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetError, "WebSocket handshaking: Failed to parse HTTP message");
                    }
                    if (!THTTP_MESSAGE_IS_RESPONSE(p_msg)) {
                        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Incoming HTTP message not a response: %.*s", endOfMessage, pcData);
                        TSK_OBJECT_SAFE_FREE(p_msg);
                        return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetError, "WebSocket handshaking: Incoming HTTP message not a response");
                    }
                    if (p_msg->line.response.status_code > 299) {
                        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Incoming HTTP response is an error: %.*s", endOfMessage, pcData);
                        TSK_OBJECT_SAFE_FREE(p_msg);
                        return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetError, "WebSocket handshaking: Incoming HTTP response is an error");
                    }
                    // Get Accept header
                    if (!(http_hdr_accept = (const thttp_header_Sec_WebSocket_Accept_t*)thttp_message_get_header(p_msg, thttp_htype_Sec_WebSocket_Accept))) {
                        CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "No 'Sec-WebSocket-Accept' header: %.*s", endOfMessage, pcData);
                        TSK_OBJECT_SAFE_FREE(p_msg);
                        return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetError, "WebSocket handshaking: No 'Sec-WebSocket-Accept' header");
                    }
                    // Authenticate the response
                    {
                        thttp_auth_ws_keystring_t resp = { 0 };
                        thttp_auth_ws_response(oPeer->getWsKey(), &resp);
                        if (!tsk_striequals(http_hdr_accept->value, resp)) {
                            CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Authentication failed: %.*s", endOfMessage, pcData);
                            TSK_OBJECT_SAFE_FREE(p_msg);
                            return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetError, "WebSocket handshaking: Authentication failed");
                        }
                    }
                    TSK_OBJECT_SAFE_FREE(p_msg);

                    const_cast<CASignaling*>(m_pcCASignaling)->m_bWsHandshakingDone = true;
                    nConsumedBytes = endOfMessage;
                    if (!m_pcCASignaling->m_bConnAuthenticated) {
                        return const_cast<CASignaling*>(m_pcCASignaling)->authConnection();
                    }
                    return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetReady, "WebSocket handshaking: done");
                }
            }
        }
    }
    else {
        /* WebSocket raw data */
        const uint8_t* pcData = (const uint8_t*)oPeer->getDataPtr();
        const uint8_t opcode = pcData[0] & 0x0F;
        if ((pcData[0] & 0x01)/* FIN */) {
            const uint8_t mask_flag = (pcData[1] >> 7); // Must be "1" for "client -> server"
            uint8_t mask_key[4] = { 0x00 };
            uint64_t pay_idx;
            uint64_t data_len = 0;
            uint64_t pay_len = 0;

            if (pcData[0] & 0x40 || pcData[0] & 0x20 || pcData[0] & 0x10) {
                CA_DEBUG_ERROR_EX(kCAMobuleNameSignaling, "Unknown extension: %d", (pcData[0] >> 4) & 0x07);
                return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetReady, "WebSocket data: Unknown extension");
            }

            pay_len = pcData[1] & 0x7F;
            data_len = 2;

            if (pay_len == 126) {
                if (oPeer->getDataSize() < 4) {
                    CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "No all data in the WS buffer");
                    nConsumedBytes = 0;
                    return true;
                }
                pay_len = (pcData[2] << 8 | pcData[3]);
                pcData = &pcData[4];
                data_len += 2;
            }
            else if (pay_len == 127) {
                if ((oPeer->getDataSize() - data_len) < 8) {
                    CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "No all data in the WS buffer");
                    nConsumedBytes = 0;
                    return true;
                }
                pay_len = (((uint64_t)pcData[2]) << 56 | ((uint64_t)pcData[3]) << 48 | ((uint64_t)pcData[4]) << 40 | ((uint64_t)pcData[5]) << 32 | ((uint64_t)pcData[6]) << 24 | ((uint64_t)pcData[7]) << 16 | ((uint64_t)pcData[8]) << 8 || ((uint64_t)pcData[9]));
                pcData = &pcData[10];
                data_len += 8;
            }
            else {
                pcData = &pcData[2];
            }

            if (mask_flag) {
                // must be "true"
                if ((oPeer->getDataSize() - data_len) < 4) {
                    CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "No all data in the WS buffer");
                    nConsumedBytes = 0;
                    return true;
                }
                mask_key[0] = pcData[0];
                mask_key[1] = pcData[1];
                mask_key[2] = pcData[2];
                mask_key[3] = pcData[3];
                pcData = &pcData[4];
                data_len += 4;
            }

            if ((oPeer->getDataSize() - data_len) < pay_len) {
                CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "No all data in the WS buffer");
                nConsumedBytes = 0;
                return true;
            }

            data_len += pay_len;
            uint8_t* _pcData = const_cast<uint8_t*>(pcData);

            // unmasking the payload
            if (mask_flag) {
                for (pay_idx = 0; pay_idx < pay_len; ++pay_idx) {
                    _pcData[pay_idx] = (pcData[pay_idx] ^ mask_key[(pay_idx & 3)]);
                }
            }
            return const_cast<CASignaling*>(m_pcCASignaling)->handleData((const char*)_pcData, (tsk_size_t)pay_len);
        }
        else if (opcode == 0x08) {
            // RFC6455 - 5.5.1.  Close
            CA_DEBUG_INFO_EX(kCAMobuleNameSignaling, "WebSocket opcode 0x8 (Close)");
            const_cast<CASignaling*>(m_pcCASignaling)->m_oNetTransport->close(oPeer->getFd());
        }
    }
    return true;
}

bool CASignalingTransportCallback::onConnectionStateChanged(CAObjWrapper<CANetPeer*> oPeer)
{
    CAAutoLock<CASignaling> autoLock(const_cast<CASignaling*>(m_pcCASignaling));

    if ((oPeer->getFd() == m_pcCASignaling->m_Fd || m_pcCASignaling->m_Fd == kCANetFdInvalid) && oPeer->isConnected()) {
        const CAWsTransport* pcTransport = dynamic_cast<const CAWsTransport*>(*m_pcCASignaling->m_oNetTransport);
        CA_ASSERT(pcTransport != NULL);
        const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetConnected, "Connected");
        if ((m_pcCASignaling->m_oConnectionUrl->getType() == CAUrlType_WS || m_pcCASignaling->m_oConnectionUrl->getType() == CAUrlType_WSS)) {
            if (!m_pcCASignaling->m_bWsHandshakingDone) {
                return const_cast<CAWsTransport*>(pcTransport)->handshaking(oPeer, const_cast<CASignaling*>(m_pcCASignaling)->m_oConnectionUrl);
            }
        }
        if (!m_pcCASignaling->m_bConnAuthenticated) {
            return const_cast<CASignaling*>(m_pcCASignaling)->authConnection();
        }
        return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetReady, "Ready");
    }
    else if (m_pcCASignaling->m_Fd == oPeer->getFd() && !oPeer->isConnected()) {
        const_cast<CASignaling*>(m_pcCASignaling)->m_Fd = kCANetFdInvalid;
        const_cast<CASignaling*>(m_pcCASignaling)->m_bWsHandshakingDone = false;
        const_cast<CASignaling*>(m_pcCASignaling)->m_bConnAuthenticated = false;
        return const_cast<CASignaling*>(m_pcCASignaling)->raiseEvent(CASignalingEventType_NetDisconnected, "Disconnected");
    }

    return true;
}

//
// CASignalingEvent
//

CASignalingEvent::CASignalingEvent(CASignalingEventType_t eType, std::string strDescription, const void* pcDataPtr /*= NULL*/, size_t nDataSize /*= 0*/)
    : m_eType(eType), m_strDescription(strDescription), m_pDataPtr(NULL), m_nDataSize(0)
{
    if (pcDataPtr && nDataSize) {
        if ((m_pDataPtr = tsk_malloc(nDataSize))) {
            memcpy(m_pDataPtr, pcDataPtr, nDataSize);
            m_nDataSize = nDataSize;
        }
    }
}

CASignalingEvent::~CASignalingEvent()
{
    TSK_FREE(m_pDataPtr);
}

//
//	CACallEvent
//

CASignalingCallEvent::CASignalingCallEvent(std::string strDescription)
    : CASignalingEvent(CASignalingEventType_Call, strDescription)
{

}

CASignalingCallEvent::~CASignalingCallEvent()
{

}