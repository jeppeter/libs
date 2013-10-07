/*! \file stdafx.h
    $Id: stdafx.h,v 1.4 2008-12-30 18:47:44 Bazis Exp $
    \brief Precompiled header file
*/

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <bzsddk/commondef.h>
extern "C" 
{
#include <ntddk.h>
#include <ntddstor.h>
#include <mountdev.h>
#include <ntddvol.h>
}
