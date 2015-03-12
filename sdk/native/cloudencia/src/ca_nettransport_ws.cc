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
#include "cloudencia/ca_nettransport_ws.h"
#include "cloudencia/ca_engine.h"
#include "cloudencia/jsoncpp/ca_json.h"

#include "tsk_string.h"
#include "tsk_memory.h"

#include "tinyhttp.h"
#include <assert.h>

/* min size of a stream chunck to form a valid HTTP message */
#define kStreamChunckMinSize 0x32
#define kHttpMethodOptions "OPTIONS"
#define kHttpMethodPost "POST"

#define kCAWsResultCode_Provisional		100
#define kCAWsResultCode_Success			200
#define kCAWsResultCode_Unauthorized		403
#define kCAWsResultCode_NotFound			404
#define kCAWsResultCode_ParsingFailed		420
#define kCAWsResultCode_InvalidDataType	483
#define kCAWsResultCode_InvalidData		450
#define kCAWsResultCode_InternalError		603

#define kCAWsResultPhrase_Success					"OK"
#define kCAWsResultPhrase_Unauthorized			"Unauthorized"
#define kCAWsResultPhrase_NotFound				"Not Found"
#define kCAWsResultPhrase_ParsingFailed			"Parsing failed"
#define kCAWsResultPhrase_InvalidDataType			"Invalid data type"
#define kCAWsResultPhrase_FailedToCreateLocalFile	"Failed to create local file"
#define kCAWsResultPhrase_FailedTransferPending	"Failed transfer pending"
#define kCAWsResultPhrase_InternalError			"Internal Error"

#define kJsonField_Action "action"
#define kJsonField_Name "name"
#define kJsonField_Type "type"



static CA_INLINE bool isJsonContentType(const thttp_message_t *pcHttpMessage);
static const char* getHttpContentType(const thttp_message_t *pcHttpMessage);

#define CA_JSON_GET(fieldParent, fieldVarName, fieldName, typeTestFun, couldBeNull) \
	if(!fieldParent.isObject()){ \
		CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "JSON '%s' not an object", (fieldName)); \
		return new CAWsResult(kCAWsResultCode_InvalidDataType, kCAWsResultPhrase_InvalidDataType); \
		} \
	const Json::Value fieldVarName = (fieldParent)[(fieldName)]; \
	if((fieldVarName).isNull()) \
		{ \
		if(!(couldBeNull)){ \
				CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "JSON '%s' is null", (fieldName)); \
				return new CAWsResult(kCAWsResultCode_InvalidDataType, "Required field is missing"); \
				}\
		} \
	if(!(fieldVarName).typeTestFun()) \
		{ \
		CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "JSON '%s' has invalid type", (fieldName)); \
		return new CAWsResult(kCAWsResultCode_InvalidDataType, kCAWsResultPhrase_InvalidDataType); \
		}

//
//	CAWsResult
//

CAWsResult::CAWsResult(unsigned short nCode, const char* pcPhrase, const void* pcDataPtr, size_t nDataSize, CAWsActionType_t eActionType)
    : m_pDataPtr(NULL)
    , m_nDataSize(0)
    , m_eActionType(eActionType)
{
    m_nCode = nCode;
    m_pPhrase = tsk_strdup(pcPhrase);
    if (pcDataPtr && nDataSize) {
        if ((m_pDataPtr = tsk_malloc(nDataSize))) {
            memcpy(m_pDataPtr, pcDataPtr, nDataSize);
            m_nDataSize = nDataSize;
        }
    }
}

CAWsResult::~CAWsResult()
{
    TSK_FREE(m_pPhrase);
    TSK_FREE(m_pDataPtr);

    CA_DEBUG_INFO("*** CAWsResult destroyed ***");
}

//
//	CAWsTransport
//

CAWsTransport::CAWsTransport(bool isSecure, const char* pcLocalIP /*= NULL*/, unsigned short nLocalPort /*= 0*/)
    : CANetTransport(isSecure ? CANetTransporType_WSS : CANetTransporType_WS, pcLocalIP, nLocalPort)
{
    m_oCallback = new CAWsTransportCallback(this);
    setCallback(*m_oCallback);
}

CAWsTransport::~CAWsTransport()
{
    setCallback(NULL);

    CA_DEBUG_INFO("*** CAWsTransport destroyed ***");
}

bool CAWsTransport::handshaking(CAObjWrapper<CANetPeer*> oPeer, CAObjWrapper<CAUrl*> oUrl)
{
    if (!isConnected(oPeer->getFd())) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "Peer with fd=%d not connected yet", oPeer->getFd());
        return false;
    }
    if (!oPeer->buildWsKey()) {
        return false;
    }

    tnet_ip_t localIP;
    tnet_port_t local_port;
    if (tnet_transport_get_ip_n_port(m_pWrappedTransport, oPeer->getFd(), &localIP, &local_port) != 0) {
        return false;
    }

    char* requestUri = tsk_null;
    if (oUrl->getHPath().empty()) {
        requestUri = tsk_strdup("/");
    }
    else {
        tsk_sprintf(&requestUri, "/%s", oUrl->getHPath().c_str());
        if (!oUrl->getSearch().empty()) {
            tsk_strcat_2(&requestUri, "?%s", oUrl->getSearch().c_str());
        }
    }

#define WS_REQUEST_GET_FORMAT "GET %s HTTP/1.1\r\n" \
	   "Host: %s\r\n" \
	   "Upgrade: websocket\r\n" \
	   "Connection: Upgrade\r\n" \
	   "Sec-WebSocket-Key: %s\r\n" \
	   "Origin: %s\r\n" \
	   "Sec-WebSocket-Protocol: ge-webrtc-signaling\r\n" \
	   "Sec-WebSocket-Version: 13\r\n" \
		"\r\n"

    char* request = tsk_null;
    tsk_sprintf(&request, WS_REQUEST_GET_FORMAT,
                requestUri,
                oUrl->getHost().c_str(),
                oPeer->getWsKey(),
                localIP);

    TSK_FREE(requestUri);

    bool ret = sendData(oPeer, request, tsk_strlen(request));
    TSK_FREE(request);
    return ret;
}

CAObjWrapper<CAWsResult*> CAWsTransport::handleJsonContent(CAObjWrapper<CANetPeer*> oPeer, const void* pcDataPtr, size_t nDataSize)const
{
    CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "Not implemented");
    return NULL;
}

bool CAWsTransport::sendResult(CAObjWrapper<CANetPeer*> oPeer, CAObjWrapper<CAWsResult*> oResult)const
{
    CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "Not implemented");
    return false;
}

//
//	CAWsTransportCallback
//
CAWsTransportCallback::CAWsTransportCallback(const CAWsTransport* pcTransport)
{
    m_pcTransport = pcTransport;
}

CAWsTransportCallback::~CAWsTransportCallback()
{

}

bool CAWsTransportCallback::onData(CAObjWrapper<CANetPeer*> oPeer, size_t &nConsumedBytes)
{
    CA_DEBUG_INFO_EX(kCAMobuleNameWsTransport, "Incoming data = %.*s", oPeer->getDataSize(), (const char*)oPeer->getDataPtr());
    CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "Not implemented");
    return false;
}

bool CAWsTransportCallback::onConnectionStateChanged(CAObjWrapper<CANetPeer*> oPeer)
{
    if (!oPeer->isConnected()) {

    }
    return true;
}

static const char* getHttpContentType(const thttp_message_t *pcHttpMessage)
{
    const thttp_header_Content_Type_t* contentType;

    if (!pcHttpMessage) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameWsTransport, "Invalid parameter");
        return NULL;
    }

    if ((contentType = (const thttp_header_Content_Type_t*)thttp_message_get_header(pcHttpMessage, thttp_htype_Content_Type))) {
        return contentType->type;
    }
    return NULL;
}

static CA_INLINE bool isContentType(const thttp_message_t *pcHttpMessage, const char* pcContentTypeToCompare)
{
    // content-type without parameters
    const char* pcContentType = getHttpContentType(pcHttpMessage);
    if (pcContentType) {
        return tsk_striequals(pcContentTypeToCompare, pcContentType);
    }
    return false;
}

static CA_INLINE bool isJsonContentType(const thttp_message_t *pcHttpMessage)
{
    return isContentType(pcHttpMessage, kJsonContentType);
}
