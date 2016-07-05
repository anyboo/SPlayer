// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

//you must WIN32_LEAN_AND_MEAN undefine becasue we use IUnknown
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef POINTER_64
#define	POINTER_64	__ptr64
#endif

// Windows Header Files:
//#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

#include <atlbase.h>
#include <atlstr.h>


#ifndef TRACE 
#define TRACE AtlTrace
#endif

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif


#define CHECK_FUNC_AND_RETURN_FALSE(func, msg) \
if (!(func)) {\
	TRACE("%s Line %d %s\n", __FILE__, __LINE__, #func); \
	MessageBox(NULL, _T("Infom"), msg,MB_OK); \
}
//return FALSE; 
#define CHECK_FUNC(func, msg) \
if (!(func)) {\
	TRACE("%s Line %d %s\n", __FILE__, __LINE__, #func); \
	ASSERT(0); \
}

//	
#undef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(ptr) \
	if (ptr) { \
		delete [] ptr; \
		ptr = NULL; \
	}
		
#undef SAFE_DELETE
#define SAFE_DELETE(ptr) \
	if (ptr) { \
		delete  ptr; \
		ptr = NULL; \
	}


// TODO: reference additional headers your program requires here
#define	MAX_IMAGE_WIDTH		2048
#define	MAX_IMAGE_HEIGHT	1024

