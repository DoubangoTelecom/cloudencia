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
#include "cloudencia/ca_utils.h"

#if CA_UNDER_WINDOWS
#	include "tinydav/tdav_win32.h"
#elif CA_UNDER_APPLE
#	include "tinydav/tdav_apple.h"
#endif

#include "tsk_uuid.h"
#include "tsk_plugin.h"

std::string CAUtils::randomString()
{
    tsk_uuidstring_t result;
    if (tsk_uuidgenerate(&result) != 0) {
        return std::string("ERROR");
    }
    return std::string(result);
}

bool CAUtils::fileExists(const char* path)
{
#define _file_exists(path) tsk_plugin_file_exist((path))
    return !!_file_exists(path);
}

const char* CAUtils::currentDirectoryPath()
{
#if SC_UNDER_WINDOWS
    return tdav_get_current_directory_const();
#else
    return ".";
#endif
}

