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

#include "tsk_md5.h"
#include "tsk_string.h"
#include "tsk_uuid.h"
#include "tsk_plugin.h"

std::string CAUtils::randomString(std::string token1 /*= ""*/, std::string token2 /*= ""*/, std::string token3 /*= ""*/, std::string token4 /*= ""*/, std::string token5 /*= ""*/, std::string token6 /*= ""*/)
{
    tsk_uuidstring_t result;
    if (tsk_uuidgenerate(&result) != 0) {
		return token1 + token2 + token3 + token4 + token5 + token6 + std::string("ERROR");
    }
	return token1 + token2 + token3 + token4 + token5 + token6 + std::string(result);
}

std::string CAUtils::itoa(int64_t i)
{
    tsk_istr_t a;
    tsk_itoa(i, &a);
    return std::string(a);
}

std::string CAUtils::buildAuthToken(std::string strLogin, std::string strPassword)
{
    // auth-token = md5('password' ':' 'login' ':' 'doubango.org');
    std::string str = strPassword + ":" + strLogin + ":" + "doubango.org";
    tsk_md5string_t md5;
    int ret;
    if ((ret = tsk_md5compute(str.c_str(), str.length(), &md5)) != 0) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameUtils, "tsk_md5compute failed with error code = %s", ret);
        return "D41D8CD98F00B204E9800998ECF8427E"; // MD5(empty string)
    }
    return std::string(md5);
}

std::string CAUtils::buildHa1(std::string strLogin, std::string strPassword, std::string strRealm)
{
    /* RFC 2617 - 3.2.2.2 A1
    A1       = unq(username-value) ":" unq(realm-value) ":" passwd
    */
    std::string str = strLogin + ":" + strRealm + ":" + strPassword;
    tsk_md5string_t md5;
    int ret;
    if ((ret = tsk_md5compute(str.c_str(), str.length(), &md5)) != 0) {
        CA_DEBUG_ERROR_EX(kCAMobuleNameUtils, "tsk_md5compute failed with error code = %s", ret);
        return "D41D8CD98F00B204E9800998ECF8427E"; // MD5(empty string)
    }
    return std::string(md5);
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

