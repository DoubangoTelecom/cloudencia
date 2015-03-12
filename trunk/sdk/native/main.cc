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
#include "cloudencia/ca_api.h"

#include <assert.h>
#include <stdio.h>
#if !CA_UNDER_WINDOWS_CE
#include <fcntl.h>
#endif /* !CA_UNDER_WINDOWS_CE */

#if CA_UNDER_WINDOWS
#include <windows.h>
#endif /* CA_UNDER_WINDOWS */

#if defined(_MSC_VER)
#	define snprintf		_snprintf
#	define vsnprintf	_vsnprintf
#	define strdup		_strdup
#	define stricmp		_stricmp
#	define strnicmp		_strnicmp
#else
#	if !HAVE_STRNICMP && !HAVE_STRICMP
#	define stricmp		strcasecmp
#	define strnicmp		strncasecmp
#	endif
#endif /* _MSC_VER */

static CAObjWrapper<CAThread*>threadConsoleReader;

#if CA_UNDER_WINDOWS
#define WM_CA_ATTACH_DISPLAYS	MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 0x0201)
static DWORD mainThreadId = 0;
#define DEFAULT_VIDEO_REMOTE_WINDOW_NAME	L"Remote video window (Decoded RTP)" // Remote window is where the decoded video frames are displayed
#define DEFAULT_VIDEO_LOCAL_WINDOW_NAME		L"Local video window (Preview)" // Local window is where the encoded video frames are displayed before sending (preview, PIP mode).
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif /* CA_UNDER_WINDOWS */

#if !defined(MAX_PATH)
#	if defined(PATH_MAX)
#		define MAX_PATH PATH_MAX
#	else
#		define MAX_PATH 260
#	endif
#endif /* MAX_PATH */

static void printHelp();
static bool loadConfig();
static char config_file_path[MAX_PATH < 260 ? 260 : MAX_PATH] = { "./config.json" };

static void* CA_STDCALL consoleReaderProc(void *arg);
static bool attachDisplays();
static CAVideoDisplay getDisplay(bool bRemote, bool bScreenCast = false);

static CAVideoDisplay displayVideoLocal = NULL;
static CAVideoDisplay displayVideoRemote = NULL;
static CAVideoDisplay displayScreenCastLocal = NULL;
static CAVideoDisplay displayScreenCastRemote = NULL;
static bool connected = false;

#if defined(_WIN32_WCE) || defined(UNDER_CE)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    CA_ASSERT(threadConsoleReader = CAThread::newObj(consoleReaderProc));

    printf("*******************************************************************\n"
           "Copyright (C) 2011-2015 Doubango Telecom (VoIP division)\n"
           "PRODUCT: CLOUDENCIA (WebRTC PaaS client native SDK)\n"
           "HOME PAGE: <void>\n"
           "CODE SOURCE: <void>\n"
           "LICENCE: <void>\n"
           "VERSION: %s\n"
           "'quit' to quit the application.\n"
           "*******************************************************************\n\n"
           , CA_VERSION_STRING);

    /* print help */
    printHelp();

#if CA_UNDER_WINDOWS
    mainThreadId = GetCurrentThreadId();
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) {
            break;
        }
        else if (msg.message == WM_CA_ATTACH_DISPLAYS) {
            CA_DEBUG_INFO("Catching 'WM_SC_ATTACH_DISPLAYS' windows event");
            CA_ASSERT(attachDisplays());
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    threadConsoleReader->join();
#endif

    threadConsoleReader = NULL;

    return 0;
}

static void printHelp()
{
    printf("----------------------------------------------------------------------------\n"
           "                               COMMANDS                                     \n"
           "----------------------------------------------------------------------------\n"
           "\"help\"               Prints this message\n"
           "\"chat [dest]\"        Audio/Video call to \"dest\" (opt., default from config.json)\n"
           "\"audio [dest]\"       Audio call to \"dest\" (opt., default from config.json)\n"
           "\"video [dest]\"       Video call to \"dest\" (opt., default from config.json)\n"
           "\"screencast [dest]\"  Share screen with \"dest\" (opt., default from config.json)\n"
           "\"mute\"               Mutes the active call\n"
           "\"unmute\"             Unmutes the active call\n"
           "\"hold\"               Holds the active call\n"
           "\"resume\"             Resumes the active call\n"
           "\"hangup\"             Terminates the active call\n"
           "\"accept\"             Accepts the incoming call\n"
           "\"reject\"             Rejects the incoming call\n"
           "\"quit\"               Teminates the application\n"
           "--------------------------------------------------------------------------\n\n"
          );
}

static void* CA_STDCALL consoleReaderProc(void *arg)
{
    char command[1024] = { 0 };
    char remoteId[25];

    (remoteId);

    CA_DEBUG_INFO("consoleReaderProc \"ENTER\"");

    while (fgets(command, sizeof(command), stdin) != NULL) {
#define CHECK_CONNECTED() if (!connected){ CA_DEBUG_INFO("+++ not connected yet +++"); continue; }
        if (strnicmp(command, "quit", 4) == 0) {
            CA_DEBUG_INFO("+++ quit() +++");
            break;
        }
        else if (strnicmp(command, "help", 4) == 0) {
            CA_DEBUG_INFO("+++ help() +++");
            printHelp();
        }
        else if (strnicmp(command, "chat", 4) == 0 || strnicmp(command, "audio", 5) == 0 || strnicmp(command, "video", 5) == 0 || strnicmp(command, "screencast", 10) == 0 || strnicmp(command, "call", 4) == 0) {

        }
        else if (strnicmp(command, "mute", 4) == 0 || strnicmp(command, "unmute", 6) == 0) {

        }
        else if (strnicmp(command, "hold", 4) == 0) {

        }
        else if (strnicmp(command, "resume", 6) == 0) {

        }
        else if (strnicmp(command, "hangup", 6) == 0 || strnicmp(command, "reject", 6) == 0) {

        }
        else if (strnicmp(command, "accept", 6) == 0) {

        }
    }

    CA_DEBUG_INFO("consoleReaderProc \"EXIT\"");

#if CA_UNDER_WINDOWS
    PostThreadMessage(mainThreadId, WM_QUIT, NULL, NULL);
#endif /* CA_UNDER_WINDOWS */

    return NULL;
}

static bool attachDisplays()
{
    CA_ASSERT(false);
    return false;
}

static CAVideoDisplay getDisplay(bool bRemote, bool bScreenCast /*= false*/)
{
#if CA_UNDER_WINDOWS
#if !defined(WS_OVERLAPPEDWINDOW)
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)
#endif /* WS_OVERLAPPEDWINDOW */
    HWND* pHWND = bRemote ? (bScreenCast ? &displayScreenCastRemote : &displayVideoRemote) : (bScreenCast ? &displayScreenCastLocal : &displayVideoLocal);
    if (!*pHWND) {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = L"MFCapture Window Class";
        /*GOTHAM_ASSERT*/(RegisterClass(&wc));

        CA_ASSERT((*pHWND = ::CreateWindow(
                                wc.lpszClassName,
                                bRemote ? DEFAULT_VIDEO_REMOTE_WINDOW_NAME : DEFAULT_VIDEO_LOCAL_WINDOW_NAME,
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                bRemote ? CW_USEDEFAULT : 352,
                                bRemote ? CW_USEDEFAULT : 288,
                                NULL,
                                NULL,
                                GetModuleHandle(NULL),
                                NULL)) != NULL);

        ::SetWindowText(*pHWND, bRemote ? DEFAULT_VIDEO_REMOTE_WINDOW_NAME : DEFAULT_VIDEO_LOCAL_WINDOW_NAME);
#if SC_UNDER_WINDOWS_CE
        ::ShowWindow(*pHWND, SW_SHOWNORMAL);
#else
        ::ShowWindow(*pHWND, SW_SHOWDEFAULT);
#endif /* SC_UNDER_WINDOWS_CE */

        ::UpdateWindow(*pHWND);
    }
    return *pHWND;
#else
    return NULL;
#endif /* SC_UNDER_WINDOWS */
}

#if CA_UNDER_WINDOWS

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
    case WM_CLOSE: {
        if (displayVideoLocal == hwnd) {
            displayVideoLocal = NULL;
        }
        else if (displayVideoRemote == hwnd) {
            displayVideoRemote = NULL;
        }
        else if (displayScreenCastLocal == hwnd) {
            displayScreenCastLocal = NULL;
        }
        else if (displayScreenCastRemote == hwnd) {
            displayScreenCastRemote = NULL;
        }
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#endif /* CA_UNDER_WINDOWS */