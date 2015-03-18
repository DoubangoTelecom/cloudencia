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

#include <string>

//
//	CAMsg
//


class CAMsg : public CAObj
{
public:
	CAMsg(std::string strType, std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string m_strTo = "");
	virtual ~CAMsg();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsg";
	}

	std::string toJson();

	static CAObjWrapper<CAMsg* > parse(std::string jsonString);
	static CAMsgType_t typeFromString(std::string strType);
	static std::string typeToString(CAMsgType_t eType);

	CA_INLINE CAMsgType_t getType() {
		return m_eType;
	}
	CA_INLINE std::string getCallId() {
		return m_strCallId;
	}

private:
	CAMsgType_t m_eType;
	std::string m_strType;
	std::string m_strCallId;
	std::string m_strTransacId;
	std::string m_strFrom;
	std::string m_strTo;
	std::string m_strAuthToken;
};

//
//	CAMsgError
//

class CAMsgError : public CAMsg {
public:
	CAMsgError(std::string strReason = "", std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string m_strTo = "");
	virtual ~CAMsgError();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgError";
	}

	CA_INLINE std::string getReason() {
		return m_strReason;
	}

private:
	std::string m_strReason;
};


//
//	CAMsgAuthConn
//

class CAMsgAuthConn : public CAMsg {
public:
	CAMsgAuthConn(std::string strFrom = "", std::string strAuthToken = "", std::string strCallId = "", std::string strTransacId = "", std::string m_strTo = "");
	virtual ~CAMsgAuthConn();
	virtual CA_INLINE const char* getObjectId() {
		return "CAMsgAuthConn";
	}

private:
};

#endif /* _CLOUDENCIA_MSG_H_ */
