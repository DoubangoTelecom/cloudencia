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
#if !defined(_CLOUDENCIA_MSG_H_)
#define _CLOUDENCIA_MSG_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_content.h"

#include <string>

namespace CAJson {
	class Value;
}

typedef struct CATicket {
	std::string type;  // kMsgFieldType
	uint64_t id; // kMsgFieldId
	CATicket() : type(""), id(kTicketNumberInvalid) { }
	bool isValid() { return  id != kTicketNumberInvalid  && !type.empty(); }
}
CATicket;

//
//	CAMsg
//


class CAMsg : public CAObj
{
public:
	CAMsg(std::string strType, std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	virtual ~CAMsg();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsg";
	}

	bool toJson(CAJson::Value* jsonValue);
	std::string toJson();
	bool setContent(CAObjWrapper<CAContent* > oContent);

	static CAObjWrapper<CAMsg* > parse(const CAJson::Value* jsonValue);
	static CAObjWrapper<CAMsg* > parse(const std::string& jsonString);
	static CAMsgType_t typeFromString(const std::string& strType);
	static std::string typeToString(CAMsgType_t eType);

	CA_INLINE CAMsgType_t getType() {
		return m_eType;
	}
	CA_INLINE std::string getCallId() {
		return m_strCallId;
	}
	CA_INLINE std::string getTransacId() {
		return m_strTransacId;
	}
	CA_INLINE CAObjWrapper<CAContent* >& getContent() {
		return m_oContent;
	}
	CA_INLINE const CATicket* getTicket() {
		return &m_Ticket;
	}

private:
	CAMsgType_t m_eType;
	std::string m_strType;
	std::string m_strCallId;
	std::string m_strTransacId;
	std::string m_strFrom;
	std::string m_strTo;
	std::string m_strAuthToken;
	CATicket m_Ticket;
	CAObjWrapper<CAContent* > m_oContent;
};

//
//	CAMsgAnswer
//

class CAMsgAnswer : public CAMsg {
protected:
	CAMsgAnswer(std::string strType, std::string strReason = "", short nCode = 0, std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	CAMsgAnswer(std::string strType, std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
public:
	virtual ~CAMsgAnswer();
	virtual bool isFor(CAObjWrapper<CAMsg* > oMsg);
	CA_INLINE std::string getReason() {
		return m_strReason;
	}
	CA_INLINE short getCode() {
		return m_nCode;
	}

private:
	std::string m_strReason;
	short m_nCode;
};

//
//	CAMsgError
//

class CAMsgError : public CAMsgAnswer {
public:
	CAMsgError(std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	CAMsgError(short nCode, std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	virtual ~CAMsgError();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgError";
	}
};

//
//	CAMsgSuccess
//

class CAMsgSuccess : public CAMsgAnswer {
public:
	CAMsgSuccess(std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	CAMsgSuccess(short nCode, std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	virtual ~CAMsgSuccess();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgSuccess";
	}
};

//
//	CAMsgProvisional
//

class CAMsgProvisional : public CAMsgAnswer {
public:
	CAMsgProvisional(std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	CAMsgProvisional(short nCode, std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	virtual ~CAMsgProvisional();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgProvisional";
	}
};


//
//	CAMsgAuthConn
//

class CAMsgAuthConn : public CAMsg {
public:
	CAMsgAuthConn(std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string strTo = "");
	virtual ~CAMsgAuthConn();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgAuthConn";
	}

private:
};

//
//	CAMsgChat
//
class CAMsgChat : public CAMsg {
public:
	CAMsgChat(std::string strFrom, std::string strAuthToken, std::string strCallId, std::string strTransacId, std::string strTo, const std::string& strContentType = "", const void* pcDataPtr = NULL, size_t nDataSize = 0, std::string strSubType = "");
	virtual ~CAMsgChat();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgChat";
	}

private:
};

#endif /* _CLOUDENCIA_MSG_H_ */
