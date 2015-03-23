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
#include "cloudencia/ca_msg.h"
#include "cloudencia/jsoncpp/ca_json.h"

#include <assert.h>


//
//	CAMsg
//


CAMsg::CAMsg(std::string strType, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
    : m_eType(typeFromString(strType))
    , m_strType(strType)
    , m_strFrom(strFrom)
    , m_strAuthToken(strAuthToken)
    , m_strCallId(strCallId)
    , m_strTransacId(strTransacId)
    , m_strTo(strTo)
{
    CA_ASSERT(!strType.empty());
#if 0
    CA_ASSERT(!strFrom.empty());
    CA_ASSERT(!strAuthToken.empty());
#endif
}

CAMsg::~CAMsg()
{

}

bool CAMsg::toJson(CAJson::Value* jsonValue)
{
	if (!jsonValue) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameMsg, "Invalid parameter");
		return false;
	}
	CAJson::Value& root = *jsonValue;
	root[kMsgFieldType] = m_strType;
	if (!m_strFrom.empty()) {
		root[kMsgFieldFrom] = m_strFrom;
	}
	if (!m_strTo.empty()) {
		root[kMsgFieldTo] = m_strTo;
	}
	if (!m_strAuthToken.empty()) {
		root[kMsgFieldAuthToken] = m_strAuthToken;
	}
	if (!m_strCallId.empty()) {
		root[kMsgFieldCallId] = m_strCallId;
	}
	if (!m_strTransacId.empty()) {
		root[kMsgFieldTransactionId] = m_strTransacId;
	}
	if (m_oContent) {
		CAJson::Value content;
		if (!m_oContent->toJson(&content)) {
			return false;
		}
		root[kMsgFieldContent] = content;
	}
	return true;
}

std::string CAMsg::toJson()
{
    CAJson::Value jsonValue;
	CA_ASSERT(toJson(&jsonValue));
    CAJson::StyledWriter writer;
	return writer.write(jsonValue);
}

bool CAMsg::setContent(CAObjWrapper<CAContent* > oContent)
{
	m_oContent = oContent;
	return true;
}

CAObjWrapper<CAMsg* > CAMsg::parse(const CAJson::Value* jsonValue)
{
	CAObjWrapper<CAMsg* > oMsg;
	CAObjWrapper<CAContent* > oContent;
	if (!jsonValue) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameMsg, "Invalid parameter");
		return NULL;
	}
	const CAJson::Value& root = *jsonValue;

	if (root[kMsgFieldType].isNull() || !root[kMsgFieldType].isString()) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameMsg, "JSON field %s is missing or invalid", kMsgFieldType);
		return NULL;
	}

	std::string strType = root[kMsgFieldType].asString();
	std::string strCallId = (!root[kMsgFieldCallId].isNull() && root[kMsgFieldCallId].isString()) ? root[kMsgFieldCallId].asString() : "";
	std::string strTransacId = (!root[kMsgFieldTransactionId].isNull() && root[kMsgFieldTransactionId].isString()) ? root[kMsgFieldTransactionId].asString() : "";
	std::string strFrom = (!root[kMsgFieldFrom].isNull() && root[kMsgFieldFrom].isString()) ? root[kMsgFieldFrom].asString() : "";
	std::string strTo = (!root[kMsgFieldTo].isNull() && root[kMsgFieldTo].isString()) ? root[kMsgFieldTo].asString() : "";
	std::string strAuthToken = (!root[kMsgFieldAuthToken].isNull() && root[kMsgFieldAuthToken].isString()) ? root[kMsgFieldAuthToken].asString() : "";

	if (!root[kMsgFieldContent].isNull() && root[kMsgFieldContent].isObject()) {
		oContent = CAContent::parse(&root[kMsgFieldContent]);
	}

	CAMsgType_t eType = typeFromString(strType);

	if (eType == CAMsgType_Error || eType == CAMsgType_Success || eType == CAMsgType_Provisional) {
		std::string strReason = (!root[kMsgFieldReason].isNull() && root[kMsgFieldReason].isString()) ? root[kMsgFieldReason].asString() : "";
		short code = (!root[kMsgFieldCode].isNull() && root[kMsgFieldCode].isIntegral()) ? root[kMsgFieldCode].asInt() : 0;
		if (eType == CAMsgType_Error) {
			oMsg = new CAMsgError(code, strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo);
		}
		else if (eType == CAMsgType_Success) {
			oMsg = new CAMsgSuccess(code, strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo);
		}
		else if (eType == CAMsgType_Provisional) {
			oMsg = new CAMsgProvisional(code, strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo);
		}
	}
	else if (eType == CAMsgType_Chat) {
		oMsg = new CAMsgChat(strFrom, strAuthToken, strCallId, strTransacId, strTo);
	}

	if (!oMsg) {
		oMsg = new CAMsg(strType, strFrom, strAuthToken, strCallId, strTransacId, strTo);
	}

	if (oMsg && oContent) {
		oMsg->setContent(oContent);
	}

	return oMsg;
}

CAObjWrapper<CAMsg* > CAMsg::parse(const std::string& jsonString)
{
    CAJson::Value root;
    CAJson::Reader reader;
	
    bool parsingSuccessful = reader.parse(jsonString, root, false);
    if (!parsingSuccessful) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameMsg, "Failed to parse JSON content: %.*s", jsonString.length(), jsonString.c_str());
        return NULL;
    }
	return parse(&root);
}

CAMsgType_t CAMsg::typeFromString(const std::string& strType)
{
    if (strType == kMsgTypeAuthConn) {
        return CAMsgType_AuthConn;
    }
    else if (strType == kMsgTypeError) {
        return CAMsgType_Error;
    }
	else if (strType == kMsgTypeSuccess) {
		return CAMsgType_Success;
	}
	else if (strType == kMsgTypeProvisional) {
		return CAMsgType_Provisional;
	}
	else if (strType == kMsgTypeChat) {
		return CAMsgType_Chat;
	}
    return CAMsgType_Unknown;
}

std::string CAMsg::typeToString(CAMsgType_t eType)
{
    switch (eType) {
    case CAMsgType_AuthConn:
        return kMsgTypeAuthConn;
	case CAMsgType_Error:
		return kMsgTypeError;
	case CAMsgType_Success:
		return kMsgTypeSuccess;
	case CAMsgType_Provisional:
		return kMsgTypeProvisional;
	case CAMsgType_Chat:
		return kMsgTypeChat;
    default:
        return kMsgTypeUnknown;
    }
}


//
//	CAMsgAnswer
//

CAMsgAnswer::CAMsgAnswer(std::string strType, std::string strReason /*= ""*/, short nCode /* = 0 */, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsg(strType, strFrom, strAuthToken, strCallId, strTransacId, strTo)
	, m_strReason(strReason)
	, m_nCode(nCode) { }

CAMsgAnswer::CAMsgAnswer(std::string strType, std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsg(strType, strFrom, strAuthToken, strCallId, strTransacId, strTo)
	, m_strReason(strReason)
	, m_nCode(0) { }

CAMsgAnswer::~CAMsgAnswer() { }

bool CAMsgAnswer::isFor(CAObjWrapper<CAMsg* > oMsg)
{
	return oMsg && oMsg->getCallId() == getCallId() && oMsg->getTransacId() == getTransacId();
}

//
//	CAMsgError
//

CAMsgError::CAMsgError(std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsgAnswer(kMsgTypeError, strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo) { }

CAMsgError::CAMsgError(short nCode, std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsgAnswer(kMsgTypeError, strReason, nCode, strFrom, strAuthToken, strCallId, strTransacId, strTo) { }

CAMsgError::~CAMsgError() { }

//
//	CAMsgSuccess
//

CAMsgSuccess::CAMsgSuccess(std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsgAnswer(kMsgTypeSuccess, strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo) { }

CAMsgSuccess::CAMsgSuccess(short nCode, std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsgAnswer(kMsgTypeSuccess, strReason, nCode, strFrom, strAuthToken, strCallId, strTransacId, strTo) { }

CAMsgSuccess::~CAMsgSuccess() { }


//
//	CAMsgProvisional
//

CAMsgProvisional::CAMsgProvisional(std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsgAnswer(kMsgTypeProvisional, strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo) { }

CAMsgProvisional::CAMsgProvisional(short nCode, std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
	: CAMsgAnswer(kMsgTypeProvisional, strReason, nCode, strFrom, strAuthToken, strCallId, strTransacId, strTo) { }

CAMsgProvisional::~CAMsgProvisional() { }


//
//	CAMsgAuthConn
//


CAMsgAuthConn::CAMsgAuthConn(std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string strTo /*= ""*/)
    : CAMsg(kMsgTypeAuthConn, strFrom, strAuthToken, strCallId, strTransacId, strTo)
{

}

CAMsgAuthConn::~CAMsgAuthConn()
{

}



//
//	CAMsgChat
//

CAMsgChat::CAMsgChat(std::string strFrom, std::string strAuthToken, std::string strCallId, std::string strTransacId, std::string strTo, const std::string& strContentType /*= ""*/, const void* pcDataPtr /*= NULL*/, size_t nDataSize /*= 0*/, std::string strSubType /*= ""*/)
	: CAMsg(kMsgTypeChat, strFrom, strAuthToken, strCallId, strTransacId, strTo)
{
	if (!strContentType.empty() && pcDataPtr && nDataSize) {
		CA_ASSERT(setContent(new CAContent(strContentType, pcDataPtr, nDataSize, strSubType)));
	}
}

CAMsgChat::~CAMsgChat()
{

}
