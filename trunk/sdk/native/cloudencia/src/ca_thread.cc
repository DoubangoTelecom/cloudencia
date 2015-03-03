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
#include "cloudencia/ca_thread.h"

#include "tsk_thread.h"

#include <assert.h>

CAThread::CAThread(CANativeThreadHandle_t* phThread)
    : m_phThread(phThread)
{
    CA_ASSERT(phThread != NULL);
}

CAThread::~CAThread()
{
    join();
    CA_DEBUG_INFO("*** CAThread destroyed ***");
}

bool CAThread::join()
{
    bool bRet = true;
    if (m_phThread) {
        CA_DEBUG_INFO("CAThread join ---ENTER---");
        bRet = (tsk_thread_join(&m_phThread) == 0);
        CA_DEBUG_INFO("CAThread join ---EXIT---");
    }
    return bRet;
}

CAObjWrapper<CAThread*> CAThread::newObj(void *(CA_STDCALL *start) (void *), void *arg /*= NULL*/)
{
    CANativeThreadHandle_t *phThread = tsk_null;
    if (tsk_thread_create(&phThread, start, arg) != 0) {
        return NULL;
    }
    return new CAThread(phThread);
}

