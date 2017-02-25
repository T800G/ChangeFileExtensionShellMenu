#ifndef _STDAFX_H__F21D569E_C913_4E37_9131_99B7517203BD_
#define _STDAFX_H__F21D569E_C913_4E37_9131_99B7517203BD_
#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif

#define STRICT
#define _ATL_APARTMENT_THREADED
#define ISOLATION_AWARE_ENABLED 1 // Enable visual styles

#include <atlbase.h>

////////////////////////////////////
// building with ATL7.1 from WDK 7
#pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")
#include <atlstdthunk.h>

////////////////////////
// DEP compatible thunk
namespace ATL
{
	inline void * __stdcall __AllocStdCallThunk()
	{
		return ::VirtualAlloc(0, sizeof(_stdcallthunk), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	}

	inline void __stdcall __FreeStdCallThunk(void *p)
	{
		if (p!=NULL) ::VirtualFree(p, 0, MEM_RELEASE);
	}
};


#include <atlapp.h>
#if (_WTL_VER < 0x0800)
#error This project requires Windows Template Library version 8.0 or higher
#endif

extern CComModule _Module;
#include <atlcom.h>
#include <atlmisc.h>
#include <atlwin.h>


#endif//_STDAFX_H__F21D569E_C913_4E37_9131_99B7517203BD_
