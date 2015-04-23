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
#include "cloudencia/ca_callbacks.h"
#include "cloudencia/ca_debug.h"

#define CA_NOCALLBACK_LOG_AND_RETURN() CA_DEBUG_INFO_EX(kCAMobuleNameCallbacks, "No callback registered for '%s'", __FUNCTION__); return true

bool CACallbackNet::raiseStateChanged(const CAObjWrapper<CACallbackNet* >&oThis, enum CANetState_e newState, const std::string& description /*= ""*/)
{
	if (oThis) {
		return oThis->onStateChanged(newState, description);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}

bool CACallbackNet::raisePassthroughData(const CAObjWrapper<CACallbackNet* >&oThis, const void* pcDataPtr, size_t nDataSize)
{
	if (oThis) {
		return oThis->onPassthroughData(pcDataPtr, nDataSize);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}

bool CACallbackChat::raiseError(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgError* > &msg)
{
	if (oThis) {
		return oThis->onError(msg);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}

bool CACallbackChat::raiseSuccess(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgSuccess* > &msg)
{
	if (oThis) {
		return oThis->onSuccess(msg);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}

bool CACallbackChat::raiseProvisional(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgProvisional* > &msg)
{
	if (oThis) {
		return oThis->onProvisional(msg);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}

bool CACallbackChat::raiseChatMessage(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgChat* > &msg)
{
	if (oThis) {
		return oThis->onChatMessage(msg);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}

bool CACallbackAuthConn::raiseAnswer(const CAObjWrapper<CACallbackAuthConn* >&oThis, CACode code, const std::string& description/* = ""*/)
{
	if (oThis) {
		return oThis->onAnswer(code, description);
	}
	CA_NOCALLBACK_LOG_AND_RETURN();
}
