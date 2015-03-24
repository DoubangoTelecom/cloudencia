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
#include "cloudencia/ca_result.h"
#include "cloudencia/jsoncpp/ca_json.h"

#include <assert.h>


//
//	CAResult
//

CAResult::CAResult(short nCode)
	: m_nCode(nCode) { }

CAResult::~CAResult() { }



//
//	CAResultTransac
//


CAResultTransac::CAResultTransac(short nCode, const std::string& strCallId, const std::string& strTransacId)
	: CAResult(nCode)
	, m_strCallId(strCallId)
	, m_strTransacId(strTransacId) { }

CAResultTransac::~CAResultTransac() { }
