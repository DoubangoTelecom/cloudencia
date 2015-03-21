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
#include "cloudencia/ca_content.h"
#include "cloudencia/jsoncpp/ca_json.h"

#include <assert.h>

#include "tsk_memory.h"

//
//	CAContent
//

CAContent::CAContent(const std::string& strType, const void* pcDataPtr /*= NULL*/, size_t nDataSize /*= 0*/, std::string strSubType /*= kContentTypeUnknown*/)
	: m_strType(strType)
	, m_eType(typeFromString(strType))
	, m_pDataPtr(NULL)
	, m_nDataSize(0)
	, m_strSubType(strSubType)
	, m_eSubType(typeFromString(strType))
{
	if (nDataSize > 0) {
		if ((m_pDataPtr = tsk_malloc(nDataSize + 1))) {
			((uint8_t*)m_pDataPtr)[nDataSize] = '\0';
			m_nDataSize = nDataSize;
		}
		if (pcDataPtr && m_pDataPtr) {
			memcpy(m_pDataPtr, pcDataPtr, nDataSize);
		}
	}
}

CAContent::~CAContent()
{
	TSK_FREE(m_pDataPtr);
}

bool CAContent::toJson(CAJson::Value* jsonValue)
{
	if (!jsonValue) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameContent, "Invalid parameter");
		return false;
	}
	CAJson::Value& root = *jsonValue;
	root[kContentFieldType] = m_strType;
	if (!m_strSubType.empty()) {
		root[kContentFieldSubType] = m_strSubType;
	}
	if (m_pDataPtr && m_nDataSize > 0) {
		root[kContentFieldData] = std::string((const char *)m_pDataPtr, m_nDataSize);
	}
	return true;
}

std::string CAContent::toJson()
{
	CAJson::Value jsonString;
	CA_ASSERT(toJson(&jsonString));
	CAJson::StyledWriter writer;
	return writer.write(jsonString);
}

CAObjWrapper<CAContent* > CAContent::parse(const CAJson::Value* jsonValue)
{
	if (!jsonValue) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameContent, "Invalid parameter");
		return NULL;
	}
	const CAJson::Value& root = *jsonValue;

	if (root[kContentFieldType].isNull() || !root[kContentFieldType].isString()) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameContent, "JSON field %s is missing or invalid", kContentFieldType);
		return NULL;
	}
	std::string strType = root[kContentFieldType].asString();
	std::string strSubType = (!root[kContentFieldSubType].isNull() && root[kContentFieldSubType].isString()) ? root[kContentFieldSubType].asString() : "";
	std::string strData = (!root[kContentFieldData].isNull() && root[kContentFieldData].isString()) ? root[kContentFieldData].asString() : "";

	CAContentType_t eType = typeFromString(strType);

	if (eType == CAContentType_Base64) {
		CA_ASSERT(false); // decode()
	}

	return new CAContent(strType, strData.empty() ? NULL : strData.c_str(), strData.length(), strSubType);
}

CAObjWrapper<CAContent* > CAContent::parse(const std::string& jsonString)
{
	CAJson::Value root;
	CAJson::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, root, false);
	if (!parsingSuccessful) {
		CA_DEBUG_ERROR_EX(kCAMobuleNameContent, "Failed to parse JSON content: %.*s", jsonString.length(), jsonString.c_str());
		return NULL;
	}
	return parse(&root);
}

CAContentType_t CAContent::typeFromString(const std::string& strType)
{
	if (strType == kContentTypeText) {
		return CAContentType_Text;
	}
	else if (strType == kContentTypeHtml) {
		return CAContentType_Html;
	}
	else if (strType == kContentTypeBase64) {
		return CAContentType_Base64;
	}
	return CAContentType_Unknown;
}

std::string CAContent::typeToString(CAContentType_t eType)
{
	switch (eType) {
	case CAContentType_Text:
		return kContentTypeText;
	case CAContentType_Html:
		return kContentTypeHtml;
	case CAContentType_Base64:
		return kContentTypeBase64;
	default:
		return kContentTypeUnknown;
	}
}
