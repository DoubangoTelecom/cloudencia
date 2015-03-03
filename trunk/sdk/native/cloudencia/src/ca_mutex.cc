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
#include "cloudencia/ca_mutex.h"

#include "tsk_mutex.h"

CAMutex::CAMutex(bool bRecursive /*= true*/)
{
    m_phOTMutex = tsk_mutex_create_2(bRecursive ? tsk_true : tsk_false);
}

CAMutex::~CAMutex()
{
    if (m_phOTMutex) {
        tsk_mutex_destroy(&m_phOTMutex);
    }
    CA_DEBUG_INFO("*** CAMutex destroyed ***");
}

bool CAMutex::lock()
{
    return (tsk_mutex_lock(m_phOTMutex) == 0);
}

bool CAMutex::unlock()
{
    return (tsk_mutex_unlock(m_phOTMutex) == 0);
}
