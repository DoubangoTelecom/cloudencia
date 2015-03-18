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


CAMsg::CAMsg(std::string strType, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string m_strTo /*= ""*/)
    : m_eType(typeFromString(strType))
    , m_strType(strType)
    , m_strFrom(strFrom)
    , m_strAuthToken(strAuthToken)
    , m_strCallId(strCallId)
    , m_strTransacId(strTransacId)
    , m_strTo(m_strTo)
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

std::string CAMsg::toJson()
{
    CAJson::Value message;
    message[kMsgFieldType] = m_strType;
    if (!m_strFrom.empty()) {
        message[kMsgFieldFrom] = m_strFrom;
    }
    if (!m_strAuthToken.empty()) {
        message[kMsgFieldAuthToken] = m_strAuthToken;
    }
    if (!m_strCallId.empty()) {
        message[kMsgFieldCallId] = m_strCallId;
    }
    if (!m_strTransacId.empty()) {
        message[kMsgFieldTransactionId] = m_strTransacId;
    }

    CAJson::StyledWriter writer;
    return writer.write(message);
}

CAObjWrapper<CAMsg* > CAMsg::parse(std::string jsonString)
{
    CAJson::Value root;
    CAJson::Reader reader;
    bool parsingSuccessful = reader.parse(jsonString, root, false);
    if (!parsingSuccessful) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameMsg, "Failed to parse JSON content: %.*s", jsonString.length(), jsonString.c_str());
        return NULL;
    }
    if (root[kMsgFieldType].isNull() || !root[kMsgFieldType].isString()) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameMsg, "JSON field %s is missing or invalid", kMsgFieldType);
        return false;
    }

    std::string strType = root[kMsgFieldType].asString();
    std::string strCallId = (!root[kMsgFieldCallId].isNull() || root[kMsgFieldCallId].isString()) ? root[kMsgFieldCallId].asString() : "";
    std::string strTransacId = (!root[kMsgFieldTransactionId].isNull() || root[kMsgFieldTransactionId].isString()) ? root[kMsgFieldTransactionId].asString() : "";
    std::string strFrom = (!root[kMsgFieldFrom].isNull() || root[kMsgFieldFrom].isString()) ? root[kMsgFieldFrom].asString() : "";
    std::string strTo = (!root[kMsgFieldTo].isNull() || root[kMsgFieldTo].isString()) ? root[kMsgFieldTo].asString() : "";
    std::string strAuthToken = (!root[kMsgFieldAuthToken].isNull() || root[kMsgFieldAuthToken].isString()) ? root[kMsgFieldAuthToken].asString() : "";

    CAMsgType_t eType = typeFromString(strType);

    if (eType == CAMsgType_Error) {
        std::string strReason = (!root[kMsgFieldReason].isNull() || root[kMsgFieldReason].isString()) ? root[kMsgFieldReason].asString() : "";
        return new CAMsgError(strReason, strFrom, strAuthToken, strCallId, strTransacId, strTo);
    }
    else {
        return new CAMsg(strType, strFrom, strAuthToken, strCallId, strTransacId, strTo);
    }
}

CAMsgType_t CAMsg::typeFromString(std::string strType)
{
    if (strType == kMsgTypeAuthConn) {
        return CAMsgType_AuthConn;
    }
    else if (strType == kMsgTypeError) {
        return CAMsgType_Error;
    }
    return CAMsgType_Unknown;
}

std::string CAMsg::typeToString(CAMsgType_t eType)
{
    switch (eType) {
    case CAMsgType_AuthConn:
        return kMsgTypeAuthConn;
    default:
        return kMsgTypeUnknown;
    }
}


//
//	CAMsgError
//

CAMsgError::CAMsgError(std::string strReason /*= ""*/, std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string m_strTo /*= ""*/)
    : CAMsg(kMsgTypeError, strFrom, strAuthToken, strCallId, strTransacId, m_strTo)
    , m_strReason(strReason)
{

}

CAMsgError::~CAMsgError()
{

}


//
//	CAMsgAuthConn
//


CAMsgAuthConn::CAMsgAuthConn(std::string strFrom /*= ""*/, std::string strAuthToken /*= ""*/, std::string strCallId /*= ""*/, std::string strTransacId /*= ""*/, std::string m_strTo /*= ""*/)
    : CAMsg(kMsgTypeAuthConn, strFrom, strAuthToken, strCallId, strTransacId, m_strTo)
{

}

CAMsgAuthConn::~CAMsgAuthConn()
{

}
