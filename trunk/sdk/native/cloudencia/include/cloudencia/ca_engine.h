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
#if !defined(_CLOUDENCIA_ENGINE_H_)
#define _CLOUDENCIA_ENGINE_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"

#include <string>

class CAEngine;

#if !defined(SWIG)
class CAIceServer : public CAObj
{
	friend class CAEngine;
protected:
	CAIceServer(
		std::string strTransport,
		std::string strServerHost,
		unsigned short serverPort,
		bool useTurn,
		bool useStun,
		std::string strUsername,
		std::string strPassword) {
		m_strTransport = strTransport;
		m_strServerHost = strServerHost;
		m_uServerPort = serverPort;
		m_bUseTurn = useTurn;
		m_bUseStun = useStun;
		m_strUsername = strUsername;
		m_strPassword = strPassword;
	}
public:
	virtual ~CAIceServer() {}
	virtual CA_INLINE const char* getObjectId() {
		return "CAIceServer";
	}
	CA_INLINE const char* getTransport()const {
		return m_strTransport.c_str();
	}
	CA_INLINE const char* getServerHost()const {
		return m_strServerHost.c_str();
	}
	CA_INLINE unsigned short getServerPort()const {
		return m_uServerPort;
	}
	CA_INLINE bool isTurnEnabled()const {
		return m_bUseTurn;
	}
	CA_INLINE bool isStunEnabled()const {
		return m_bUseStun;
	}
	CA_INLINE const char* getUsername()const {
		return m_strUsername.c_str();
	}
	CA_INLINE const char* getPassword()const {
		return m_strPassword.c_str();
	}

private:
	std::string m_strTransport;
	std::string m_strServerHost;
	unsigned short m_uServerPort;
	bool m_bUseTurn;
	bool m_bUseStun;
	std::string m_strUsername;
	std::string m_strPassword;
};

#endif /* !defined(SWIG) */

class CLOUDENCIA_API CAEngine : public CAObj
{
private:
	CAEngine();
public:
	virtual ~CAEngine();
	virtual CA_INLINE const char* getObjectId() {
		return "CAEngine";
	}

	static bool init(std::string strCredUserId = "", std::string strCredPassword = "");
	static bool deInit();
	static bool isInitialized() {
		return s_bInitialized;
	}
	CA_DISABLE_WARNINGS_BEGIN(4251 4267)
private:
	static bool s_bInitialized;
	static std::string s_strCredUserId;
	static std::string s_strCredPassword;
	CA_DISABLE_WARNINGS_END()
};

#endif /* _CLOUDENCIA_ENGINE_H_ */
