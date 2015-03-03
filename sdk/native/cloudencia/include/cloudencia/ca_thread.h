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
#if !defined(_CLOUDENCIA_THREAD_H_)
#define _CLOUDENCIA_THREAD_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"

class CLOUDENCIA_API CAThread : public CAObj
{
protected:
	CAThread(CANativeThreadHandle_t* phThread);
public:
	virtual ~CAThread();
	virtual CA_INLINE const char* getObjectId() {
		return "CAThread";
	}
	bool join();
	static CAObjWrapper<CAThread*> newObj(void *(CA_STDCALL *start) (void *), void *arg = NULL);

private:
	CANativeThreadHandle_t* m_phThread;
};

#endif /* _CLOUDENCIA_THREAD_H_ */
