#if !defined (WINMAIN_H_CA52C692_38BA_4E31_9A2C_89B6B36FE073)
#define WINMAIN_H_CA52C692_38BA_4E31_9A2C_89B6B36FE073

// standard definitions
#define STRICT												// enable strict type-checking of Windows handles
#define WIN32_LEAN_AND_MEAN									// allow the exclusion of uncommon features
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES			1	// use the new secure functions in the CRT
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT	1	// use the new secure functions in the CRT

// standard includes
#include <windows.h>			// fundamental Windows header file
#include <windowsx.h>			// useful Windows programming extensions
#include <tchar.h>				// generic text character mapping
#include <string.h>				// includes string manipulation routines
#include <stdlib.h>				// includes standard libraries
#include <stdio.h>				// includes standard input/output routines
#include <process.h>			// threading routines for the CRT
#include <gl\gl.h>				// the OpenGL Library
#include <gl\glu.h>				// the OpenGL Utility Library

#pragma warning (disable:4100)	// unreferenced formal parameter

#ifndef __cplusplus
	// mimic a boolean type in C
	typedef enum {false=FALSE, true=TRUE} bool;
#endif

// app configs and defaults
#define APP_ALLOW_FULLSCREEN	false												// should the app allow fullscreen mode
#define APP_ALLOW_RESIZE		false												// can the main window to be resized? (windowed only)
#define APP_ALLOW_MENU			false												// do we have a default menu
#define APP_ALLOW_VSYNC			false												// do allow the enabling/disabling of vertical sync?
#define APP_CLASSNAME			_T("GLSample_9826C328_598D_4C2E_85D4_0FF8E0310366")	// classname of the main application window
#define APP_DEF_BPP				8													// default bits-per-pixel
#define APP_DEF_FULLSCREEN		false												// should the app default to fullscreen or windowed
#define APP_DEF_HEIGHT			480													// default height of the resolution
#define APP_DEF_WIDTH			640													// default width of the resolution
#define APP_MAX_REFRESH			120													// default max refresh rate to use for fullscreen mode (in hertz)
#define APP_MIN_REFRESH			60													// default min refresh rate to use for fullscreen mode (in hertz)
#define APP_MIN_WINVER_MAJOR	4													// min os version the app can run on
#define APP_MIN_WINVER_MINOR	0													// min os version the app can run on
#define APP_MIN_HEIGHT			0													// mininum height of the main window (zero means no min)
#define APP_MIN_WIDTH			0													// mininum width of the main window (zero means no min)
#define APP_NAME				_T("OpenGL Sample")									// name of the application (independent of window title)
#define APP_SINGLE_INSTANCE		true												// do we allow single or multiple instances of the app
#define APP_WINVER_NTONLY		false												// should app be ran on NT only?
#define COMPANY_NAME			_T("Wizkit")										// name of the company responsible for this software
#define MAX_LOADSTRING			256													// max buffer to load for simple string data

// global app specific includes
#include "Resource.h"		// resource identifier declarations

#endif  // WINMAIN_H
