#include "stdafx.h"
#include "processviewer.h"
#include "OSVer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

OSVer::OSVer()
{
    ZeroMemory( &m_stOsVer, sizeof( m_stOsVer ));
    m_stOsVer.dwOSVersionInfoSize = sizeof( m_stOsVer );
    GetVersionEx( &m_stOsVer );
}

OSVer::~OSVer()
{}

LPCTSTR OSVer::GetServicePack() const
{
   return GetOSVer().szCSDVersion;
}

bool OSVer::Is2003() const
{
   return ( GetOSVer().dwPlatformId    == VER_PLATFORM_WIN32_NT &&
            GetOSVer().dwMajorVersion  == 5 &&
            GetOSVer().dwMinorVersion  == 2 );
}

bool OSVer::IsXP() const
{
    return ( GetOSVer().dwPlatformId   == VER_PLATFORM_WIN32_NT && 
             GetOSVer().dwMajorVersion == 5 && 
             GetOSVer().dwMinorVersion == 1 );
}

bool OSVer::Is2000() const
{
    return ( GetOSVer().dwPlatformId   == VER_PLATFORM_WIN32_NT &&
             GetOSVer().dwMajorVersion == 5 && 
             GetOSVer().dwMinorVersion == 0 );
}

bool OSVer::IsWin95() const
{
    return ( GetOSVer().dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS &&
             GetOSVer().dwMajorVersion == 4 &&
             GetOSVer().dwMinorVersion == 0 );
}

bool OSVer::IsWin98() const
{
    return ( GetOSVer().dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS &&
             GetOSVer().dwMajorVersion == 4 &&
             GetOSVer().dwMinorVersion == 10 );
}

bool OSVer::IsWinME() const
{
    return ( GetOSVer().dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS &&
             GetOSVer().dwMajorVersion == 4 &&
             GetOSVer().dwMinorVersion == 90 );
}

const OSVERSIONINFO& OSVer::GetOSVer() const
{
    return m_stOsVer;
}