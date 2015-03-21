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
#if !defined(_CLOUDENCIA_CONTENT_H_)
#define _CLOUDENCIA_CONTENT_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"

#include <string>

namespace CAJson {
	class Value;
}

class CAContent : public CAObj
{
public:
	CAContent(const std::string& strType, const void* pcDataPtr = NULL, size_t nDataSize = 0, std::string strSubType = "");
	virtual ~CAContent();
	virtual CA_INLINE const char* getObjectId() {
		return "CAContent";
	}
	bool toJson(CAJson::Value* jsonValue);
	std::string toJson();

	static CAObjWrapper<CAContent* > parse(const CAJson::Value* jsonValue);
	static CAObjWrapper<CAContent* > parse(const std::string& jsonString);
	static CAContentType_t typeFromString(const std::string& strType);
	static std::string typeToString(CAContentType_t eType);

	CA_INLINE CAContentType_t getType() {
		return m_eType;
	}
	CA_INLINE CAContentType_t getSubType() {
		return m_eSubType;
	}
	CA_INLINE const void* getDataPtr() {
		return m_pDataPtr;
	}
	CA_INLINE size_t getDataSize() {
		return m_nDataSize;
	}

private:
	std::string m_strType;
	std::string m_strSubType;
	CAContentType_t m_eType;
	CAContentType_t m_eSubType;
	void* m_pDataPtr;
	size_t m_nDataSize;
};

#endif /* _CLOUDENCIA_CONTENT_H_ */

