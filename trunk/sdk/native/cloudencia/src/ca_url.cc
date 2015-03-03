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
#include "cloudencia/CA_url.h"

#include "tsk_string.h"

CAUrl::CAUrl(CAUrlType_t eType, std::string strScheme, std::string strHost, std::string strHPath, std::string strSearch, unsigned short nPort, CAUrlHostType_t eHostType)
    : m_eType(eType)
    , m_strScheme(strScheme)
    , m_strHost(strHost)
    , m_strHPath(strHPath)
    , m_strSearch(strSearch)
    , m_nPort(nPort)
    , m_eHostType(eHostType)
{

}

CAUrl::~CAUrl()
{
    CA_DEBUG_INFO("*** CAUrl destroyed ***");
}

CAObjWrapper<CAUrl*> CAUrl::newObj(CAUrlType_t eType, const char* pcCAheme, const char* pcHost, const char* pcHPath, const char* pcSearch, unsigned short port, CAUrlHostType_t eHostType)
{
    if (tsk_strnullORempty(pcCAheme)) {
        CA_DEBUG_ERROR("Url CAheme is null or empty");
        return NULL;
    }
    if (tsk_strnullORempty(pcHost)) {
        CA_DEBUG_ERROR("Url host is null or empty");
        return NULL;
    }
    if (!port) {
        CA_DEBUG_ERROR("Url port is equal to zero");
        return NULL;
    }

    return new CAUrl(
               eType,
               std::string(pcCAheme),
               std::string(pcHost),
               std::string(pcHPath ? pcHPath : ""),
               std::string(pcSearch ? pcSearch : ""),
               port,
               eHostType);
}
