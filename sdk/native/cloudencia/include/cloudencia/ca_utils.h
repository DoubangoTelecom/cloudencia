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
#ifndef _CLOUDENCIA_UTILS_H_
#define _CLOUDENCIA_UTILS_H_

#include "ca_config.h"
#include "cloudencia/ca_common.h"

#include <string>

class CLOUDENCIA_API CAUtils
{
public:
    static std::string randomString();
    static bool fileExists(const char* path);
    static const char* currentDirectoryPath();
};

#endif /* _CLOUDENCIA_UTILS_H_ */
