#ifndef _AUTO_START_UP_INFO_H_
#define _AUTO_START_UP_INFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TypedefsInclude.h"

class CAutoStartupInfo  
{
    public:

	    CAutoStartupInfo();
	    virtual ~CAutoStartupInfo();
        bool Load();
        bool Lookup( LPCTSTR lpctszExeName_i ) const;
        void AddAutoStartExePath( const CString& csAutoStartExePath_i )
        {
            m_AutoStartups.push_back( csAutoStartExePath_i );
        }

    private:

        // Registry helper functions
        HKEY GetRegKey( const HKEY hKey, LPCTSTR lpctszSubKey ) const;
        bool GetAutoStartupApplicationPaths( const HKEY hKey_i );
        bool IsShortPathName( LPCTSTR lpctszPath_i ) const
        {
            return lpctszPath_i && _tcschr( lpctszPath_i, _T( '~' ));
        }
        bool IsEnvironmentVarPath( LPCTSTR lpctszPath_i ) const
        {
            return lpctszPath_i && _tcschr( lpctszPath_i, _T( '%' ));
        }

        // Get's string value of a key's value from registry
        bool GetRegKeyStringValue( const HKEY hKey_i, LPCTSTR lpctszSubKeyName_i, CString& csValue_o ) const;

        // Reads in users profile directory
        bool GetUserProfileDir( CString& csUserProfileDir_o,
                                CString& csAllUserProfileDir_o ) const;
        bool GetStartupFolderPath( CString& csStartupFolderPath_o,
                                   CString& csAllUsersStartupFolderPath_o ) const;

        bool FindExeFromPath( CString csPath_i );
        bool ReadInAutoStartAppsFromStartupFolder();

    private: // Member vars

        StringVector m_AutoStartups;

        HKEY m_hLocalMachineAutostartupKey;
        HKEY m_hCurrentUserAutostartupKey;
};

#endif // _AUTO_START_UP_INFO_H_
