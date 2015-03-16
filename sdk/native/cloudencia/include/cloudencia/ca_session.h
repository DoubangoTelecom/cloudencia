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
#if !defined(_CLOUDENCIA_SESSION_H_)
#define _CLOUDENCIA_SESSION_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_signaling.h"

class CASession : public CAObj
{
protected:
	CASession(CASessionType_t eType, CAObjWrapper<CASignaling* > oSignaling);
public:
	virtual ~CASession();
	virtual CA_INLINE const char* getObjectId() {
		return "SCSession";
	}

	virtual CA_INLINE CASessionType_t getType()const {
		return m_eType;
	}

protected:
	CAObjWrapper<CASignaling* > m_oSignaling;

private:
	CASessionType_t m_eType;
};

#endif /* _CLOUDENCIA_SESSION_H_ */
