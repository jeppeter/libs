// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A64F1206_868F_4275_BFA5_A407CC22C851__INCLUDED_)
#define AFX_STDAFX_H__A64F1206_868F_4275_BFA5_A407CC22C851__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define _CRT_SECURE_NO_DEPRECATE

// If unicode defined then use unicode version functions of dbghelp API's
#if defined _UNICODE || defined UNICODE
	#define DBGHELP_TRANSLATE_TCHAR
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "AggressiveOptimize.h"
#include "Utils.h"
#include "Settings.h"
#include "Log.h"

class Settings;
extern Settings& g_Settings;

// Global Log instance
extern Log g_Log;

#pragma warning( push, 3 )
#include <vector>
#include <string>
#pragma warning( pop )


#define MAKE_CASE_STR( val )\
    case val: \
    {\
        static LPCTSTR lpctsz##val = _T( #val ); \
        return lpctsz##val;\
    }


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A64F1206_868F_4275_BFA5_A407CC22C851__INCLUDED_)