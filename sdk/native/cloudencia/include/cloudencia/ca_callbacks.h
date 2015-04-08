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
#if !defined(_CLOUDENCIA_CALLBACKS_H_)
#define _CLOUDENCIA_CALLBACKS_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"
#include "cloudencia/ca_obj.h"
#include "cloudencia/ca_msg.h"

#include <string>

class CASignaling;
class CASignalingTransportCallback;

/**@ingroup _Group_CPP_Callbacks
* Callback definition
*/
class CLOUDENCIA_API CACallbackNet : public CAObj
{
	friend class CASignaling;
	friend class CASignalingTransportCallback;
protected:
	CACallbackNet() { };
public:
	virtual CA_INLINE const char* getObjectId() { return "CACallbackNet"; }
	virtual bool onStateChanged(enum CANetState_e newState, const std::string& description = "")const = 0;
	virtual bool onPassthroughData(const void* pcDataPtr, size_t nDataSize)const = 0;

private:
	static bool raiseStateChanged(const CAObjWrapper<CACallbackNet* >&oThis, enum CANetState_e newState, const std::string& description = "");
	static bool raisePassthroughData(const CAObjWrapper<CACallbackNet* >&oThis, const void* pcDataPtr, size_t nDataSize);
};


/**@ingroup _Group_CPP_Callbacks
* Callback definition
*/
class CLOUDENCIA_API CACallbackChat : public CAObj
{
	friend class CASignaling;
protected:
	CACallbackChat() { };
public:
	virtual CA_INLINE const char* getObjectId() { return "CACallbackChat"; }
	virtual bool onError(const CAObjWrapper<CAMsgError* > &msg)const = 0;
	virtual bool onSuccess(const CAObjWrapper<CAMsgSuccess* > &msg)const = 0;
	virtual bool onProvisional(const CAObjWrapper<CAMsgProvisional* > &msg)const = 0;
	virtual bool onChatMessage(const CAObjWrapper<CAMsgChat* > &msg)const = 0;

private:
	static bool raiseError(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgError* > &msg);
	static bool raiseSuccess(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgSuccess* > &msg);
	static bool raiseProvisional(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgProvisional* > &msg);
	static bool raiseChatMessage(const CAObjWrapper<CACallbackChat* >&oThis, const CAObjWrapper<CAMsgChat* > &msg);
};

/**@ingroup _Group_CPP_Callbacks
* Callback definition
*/
class CLOUDENCIA_API CACallbackAuthConn : public CAObj
{
	friend class CASignaling;
protected:
	CACallbackAuthConn() { };
public:
	virtual CA_INLINE const char* getObjectId() { return "CACallbackAuthConn"; }
	virtual bool onAnswer(CACode code, const std::string & description = "")const = 0;

private:
	static bool raiseAnswer(const CAObjWrapper<CACallbackAuthConn* >&oThis, CACode code, const std::string& description = "");
};

#endif /* _CLOUDENCIA_CALLBACKS_H_ */
