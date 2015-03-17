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
#if !defined(_CLOUDENCIA_URL_H_)
#define _CLOUDENCIA_URL_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"

#include <string>

class CAUrl : public CAObj
{
protected:
    CAUrl(CAUrlType_t eType, std::string strScheme, std::string strHost, std::string strHPath, std::string strSearch, unsigned short nPort, CAUrlHostType_t eHostType);
public:
    virtual ~CAUrl();
    virtual CA_INLINE const char* getObjectId() {
        return "CAUrl";
    }

    virtual CA_INLINE CAUrlType_t getType()const {
        return m_eType;
    }
    virtual CA_INLINE CAUrlHostType_t getHostType()const {
        return m_eHostType;
    }
    virtual CA_INLINE std::string getScheme()const {
        return m_strScheme;
    }
    virtual CA_INLINE std::string getHost()const {
        return m_strHost;
    }
    virtual CA_INLINE std::string getHPath()const {
        return m_strHPath;
    }
    virtual CA_INLINE std::string getSearch()const {
        return m_strSearch;
    }
    virtual CA_INLINE unsigned short getPort()const {
        return m_nPort;
    }

    static CAObjWrapper<CAUrl*> newObj(CAUrlType_t eType, const char* pcScheme, const char* pcHost, const char* pcHPath, const char* pcSearch, unsigned short port, CAUrlHostType_t eHostType);

private:
    CAUrlType_t m_eType;
    CAUrlHostType_t m_eHostType;
    std::string m_strScheme;
    std::string m_strHost;
    std::string m_strHPath;
    std::string m_strSearch;
    unsigned short m_nPort;
};

#endif /* _CLOUDENCIA_URL_H_ */
