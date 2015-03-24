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
#if !defined(_CLOUDENCIA_RESULT_H_)
#define _CLOUDENCIA_RESULT_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"

#include <string>

//
//	CAResult
//


class CLOUDENCIA_API CAResult : public CAObj
{
public:
	CAResult(short nCode);
	virtual ~CAResult();
	virtual CA_INLINE const char* getObjectId() {
		return "CAResult";
	}

	virtual CA_INLINE short getCode() {
		return m_nCode;
	}
	virtual CA_INLINE bool isProvisional() {
		return m_nCode < 200;
	}
	virtual CA_INLINE bool isSuccess() {
		return m_nCode < 300 && m_nCode > 199;
	}
	virtual CA_INLINE bool isError() {
		return m_nCode > 299;
	}
	virtual CA_INLINE bool isOk() {
		return isProvisional() || isSuccess();
	}
	virtual CA_INLINE bool isNOk() {
		return !isOk();
	}

private:
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
	short m_nCode;
	CA_DISABLE_WARNINGS_END()
};

//
//	CAResultTransac
//

class CLOUDENCIA_API CAResultTransac : public CAResult
{
public:
	CAResultTransac(short nCode, const std::string& strCallId, const std::string& strTransacId);
	virtual ~CAResultTransac();
	virtual CA_INLINE const char* getObjectId() {
		return "CAResultTransac";
	}

	virtual CA_INLINE std::string getCallId() {
		return m_strCallId;
	}
	virtual CA_INLINE std::string getTransacId() {
		return m_strTransacId;
	}

private:
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
	std::string m_strCallId;
	std::string m_strTransacId;
	CA_DISABLE_WARNINGS_END()
};

#endif /* _CLOUDENCIA_RESULT_H_ */
