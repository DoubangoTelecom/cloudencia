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
#ifndef _CLOUDENCIA_CONFIG_H_
#define _CLOUDENCIA_CONFIG_H_

#define CA_VERSION_MAJOR 1
#define CA_VERSION_MINOR 0
#define CA_VERSION_MICRO 0
#if !defined(CA_VERSION_STRING)
#	define CA_VERSION_STRING CA_STRING(CA_CAT(CA_VERSION_MAJOR, .)) CA_STRING(CA_CAT(CA_VERSION_MINOR, .)) CA_STRING(CA_VERSION_MICRO)
#endif

// Windows (XP/Vista/7/CE and Windows Mobile) macro definition.
#if defined(WIN32)|| defined(_WIN32) || defined(_WIN32_WCE)
#	define CA_UNDER_WINDOWS	1
#	if defined(_WIN32_WCE) || defined(UNDER_CE)
#		define CA_UNDER_WINDOWS_CE	1
#		define CA_STDCALL			__cdecl
#	else
#		define CA_STDCALL __stdcall
#	endif
#	if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP || WINAPI_FAMILY == WINAPI_FAMILY_APP)
#		define CA_UNDER_WINDOWS_RT		1
#	endif
#else
#	define CA_STDCALL
#endif
// OSX or iOS
#if defined(__APPLE__)
#	define CA_UNDER_APPLE				1
#endif
#if TARGET_OS_MAC
#	define CA_UNDER_MAC					1
#endif
#if TARGET_OS_IPHONE
#	define CA_UNDER_IPHONE				1
#endif
#if TARGET_IPHONE_SIMULATOR
#	define CA_UNDER_IPHONE_SIMULATOR	1
#endif


// Disable some well-known warnings
#if defined(_MSC_VER)
#if !defined(_CRT_SECURE_NO_WARNINGS)
#		define _CRT_SECURE_NO_WARNINGS
#	endif /* _CRT_SECURE_NO_WARNINGS */
#	define CA_INLINE	_inline
#else
#	define CA_INLINE	inline
#endif

// Disable some well-known warnings
#if defined(_MSC_VER)
#	define CA_DISABLE_WARNINGS_BEGIN(nn) \
		__pragma(warning( push )) \
		__pragma(warning( disable : ##nn ))
#	define CA_DISABLE_WARNINGS_END() \
		__pragma(warning( pop ))
#else
#	define CA_DISABLE_WARNINGS_BEGIN(nn)
#	define CA_DISABLE_WARNINGS_END()
#endif /* _MSC_VER */


#if CA_UNDER_WINDOWS && defined(CLOUDENCIA_EXPORTS)
# 	define CLOUDENCIA_API		__declspec(dllexport)
# 	define CLOUDENCIA_GEXTERN	__declspec(dllexport)
#elif CA_UNDER_WINDOWS /*&& defined(CLOUDENCIA_IMPORTS)*/
# 	define CLOUDENCIA_API		__declspec(dllimport)
# 	define CLOUDENCIA_GEXTERN	__declspec(dllimport)
#else
#	define CLOUDENCIA_API
#	define CLOUDENCIA_GEXTERN	extern
#endif

#ifdef __GNUC__
#	define ca_atomic_inc(_ptr_) __sync_fetch_and_add((_ptr_), 1)
#	define ca_atomic_dec(_ptr_) __sync_fetch_and_sub((_ptr_), 1)
#elif defined (_MCA_VER)
#	define ca_atomic_inc(_ptr_) InterlockedIncrement((_ptr_))
#	define ca_atomic_dec(_ptr_) InterlockedDecrement((_ptr_))
#else
#	define ca_atomic_inc(_ptr_) ++(*(_ptr_))
#	define ca_atomic_dec(_ptr_) --(*(_ptr_))
#endif


/* define "TNET_DEPRECATED(func)" macro */
#if defined(__GNUC__)
#	define CA_DEPRECATED(func) __attribute__ ((deprecated)) func
#elif defined(_MCA_VER)
#	define CA_DEPRECATED(func) __declspec(deprecated) func
#else
#	pragma message("WARNING: Deprecated not supported for this compiler")
#	define CA_DEPRECATED(func) func
#endif

#if CA_UNDER_WINDOWS
#	define _WINSOCKAPI_
#	include <windows.h>
#elif CA_UNDER_LINUX
#elif CA_UNDER_MACOS
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#endif /* _CLOUDENCIA_CONFIG_H_ */
