#include "stdafx.h"
#include "AutoStartupInfo.h"
#include <UserEnv.h>

//************************************
// Method:    CAutoStartupInfo
// FullName:  CAutoStartupInfo::CAutoStartupInfo
// Access:    public 
// Returns:   
// Qualifier: : m_hLocalMachineAutostartupKey( 0 )*/, m_hCurrentUserAutostartupKey( 0 )
//************************************
CAutoStartupInfo::CAutoStartupInfo()
    :   m_hLocalMachineAutostartupKey( 0 ),
        m_hCurrentUserAutostartupKey( 0 )
{
    // Reserve space for at least 30 entries
    m_AutoStartups.reserve( 30 );

    LPCTSTR lpctszAutorunKeyPath =  _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\" );
    m_hLocalMachineAutostartupKey = GetRegKey( HKEY_LOCAL_MACHINE, 
                                               lpctszAutorunKeyPath );
    ASSERT( m_hLocalMachineAutostartupKey );

    m_hCurrentUserAutostartupKey = GetRegKey( HKEY_CURRENT_USER,
                                              lpctszAutorunKeyPath );
    ASSERT( m_hCurrentUserAutostartupKey );
}


//************************************
// Method:    ~CAutoStartupInfo
// FullName:  CAutoStartupInfo::~CAutoStartupInfo
// Access:    public 
// Returns:   
// Qualifier:
//************************************
CAutoStartupInfo::~CAutoStartupInfo()
{
   // Close local machine auto startup key
   RegCloseKey( m_hLocalMachineAutostartupKey );

   // Close current user auto start up key
   RegCloseKey( m_hCurrentUserAutostartupKey );
}


//************************************
// Method:    Load
// FullName:  CAutoStartupInfo::Load
// Access:    public 
// Returns:   bool
// Qualifier:
//************************************
bool CAutoStartupInfo::Load()
{
    // Clear previous entries
    m_AutoStartups.clear();

    if( m_hLocalMachineAutostartupKey )
    {
        if( !GetAutoStartupApplicationPaths( m_hLocalMachineAutostartupKey ))
        {
            TRACE_ERR( "Failed to read in auto run applications from local machine run registry key" );
        }
    }// End if

    if( m_hCurrentUserAutostartupKey )
    {
        if( !GetAutoStartupApplicationPaths( m_hCurrentUserAutostartupKey ))
        {
            TRACE_ERR( "Failed to read in auto run applications from current user run registry key" );
        }
    }

    if( !ReadInAutoStartAppsFromStartupFolder() )
    {
        TRACE_ERR( "Failed to read in auto start apps from startup folder in start menu" );
    }

    return true;
}// End Load


//************************************
// Method:    Lookup
// FullName:  CAutoStartupInfo::Lookup
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: LPCTSTR lpctszExePath_i
//************************************
bool CAutoStartupInfo::Lookup( LPCTSTR lpctszExePath_i ) const
{
    if( !lpctszExePath_i )
    {
        return false;
    }

    // Loop through string vector and find out if we have an entry for this exe
    for( SVCItr StringItr = m_AutoStartups.begin(); 
         StringItr != m_AutoStartups.end(); 
         ++StringItr )
    {
        if( StrStrI( *StringItr, lpctszExePath_i ))
        {
            return true;
        }// End if
    }// End for

    return false;
}


//************************************
// Method:    GetRegKey
// FullName:  CAutoStartupInfo::GetRegKey
// Access:    private 
// Returns:   HKEY
// Qualifier:
// Parameter: HKEY hKey
// Parameter: LPCTSTR lpctszSubKey_i
//************************************
HKEY CAutoStartupInfo::GetRegKey( const HKEY hKey, LPCTSTR lpctszSubKey_i ) const
{
    HKEY hSubKey = 0;
    if( RegOpenKeyEx( hKey, 
                      lpctszSubKey_i,
                      0,
                      KEY_READ,
                      &hSubKey ) == ERROR_SUCCESS )
    {
        return hSubKey;
    }
    else
    {
        return NULL;
    }// End if
}// End GetAutoStartupKey


//************************************
// Method:    GetAutoStartupApplicationPaths
// FullName:  CAutoStartupInfo::GetAutoStartupApplicationPaths
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: HKEY hKey_i
// Parameter: StringVector & KeyValues_io
//************************************
bool CAutoStartupInfo::GetAutoStartupApplicationPaths( const HKEY hKey_i )
{
    // Key name and key value buffers
    TCHAR szKeyNameBuff[MAX_PATH] = { 0 };
    DWORD dwKeyNameLength = MAX_PATH;

    for( DWORD dwIndex = 0; 
         RegEnumValue( hKey_i, dwIndex, szKeyNameBuff, &dwKeyNameLength, 0, 0, 0, 0 ) == ERROR_SUCCESS;
         ++dwIndex )
    {
        CString csKeyValue;
        if( GetRegKeyStringValue( hKey_i, szKeyNameBuff, csKeyValue ))
        {
            if( IsEnvironmentVarPath( csKeyValue ))
            {
                // Get expanded path
                TCHAR szExpandedPath[MAX_PATH] = { 0 };
                ExpandEnvironmentStrings( csKeyValue, szExpandedPath, MAX_PATH );

                // Copy back to original buffer
                csKeyValue = szExpandedPath;
            }

            LPTSTR lptszKeyValueBuff = csKeyValue.GetBufferSetLength( csKeyValue.GetLength() + MAX_PATH );

            // If path is unquoted then quote, because PathRemoveArgs requires paths with spaces to
            // be quoted
            if( !_tcschr( lptszKeyValueBuff, _T( '\"' )))
            {
               PathQuoteSpaces( lptszKeyValueBuff );
            }

            // Remove command line arguments from path
            PathRemoveArgs( lptszKeyValueBuff );

            // Check whether file exists or not, searches for file in the standard paths
            // that are set
            BOOL bFound = PathFileExists( lptszKeyValueBuff );

            // Removes any quotes from path
            PathUnquoteSpaces( lptszKeyValueBuff );

            // If file is a shortcut then resolve to real path
            if( bFound && 
                Utils::IsShortcut( lptszKeyValueBuff ))
            {
                bFound = Utils::ResolveShortcut( lptszKeyValueBuff );
            }

            csKeyValue.ReleaseBuffer();
            AddAutoStartExePath( csKeyValue );
        }// End if

        dwKeyNameLength = MAX_PATH;
    }// End for

    return true;
}// End GetAutoStartupApplicationPaths


//************************************
// Method:    GetRegKeyStringValue
// FullName:  CAutoStartupInfo::GetRegKeyStringValue
// Access:    private 
// Returns:   bool
// Qualifier: const
// Parameter: const HKEY hKey_i
// Parameter: LPCTSTR lpctszSubKeyName_i
// Parameter: CString & csValue_o
//************************************
bool CAutoStartupInfo::GetRegKeyStringValue( const HKEY hKey_i, LPCTSTR lpctszSubKeyName_i, CString& csValue_o ) const
{
    if( !hKey_i || !lpctszSubKeyName_i )
    {
        return false;
    }

    DWORD dwKeyType = 0;

    DWORD dwKeyValueBuffByteLen = MAX_PATH * sizeof( TCHAR );
    TCHAR szKeyValue[MAX_PATH] = { 0 };

    // Get key value
    const long lRes = RegQueryValueEx( hKey_i, 
                                       lpctszSubKeyName_i, 
                                       0, 
                                       &dwKeyType, 
                                       RCAST( LPBYTE, szKeyValue ), 
                                       &dwKeyValueBuffByteLen );


    // Check return value
    if( lRes != ERROR_SUCCESS )
    {
        return false;
    }

    // Check key type
    if( dwKeyType != REG_SZ && dwKeyType != REG_EXPAND_SZ )
    {
        return false;
    }

    if( dwKeyValueBuffByteLen != 0 )
    {
       if(( dwKeyValueBuffByteLen % sizeof( TCHAR ) != 0 ) || 
          ( szKeyValue[dwKeyValueBuffByteLen / sizeof(TCHAR) - 1] != 0 ))
       {
          return false;
       }
       else
       {
          csValue_o = szKeyValue;
       }// End if
    }// End if

    return true;
}//End GetRegKeyStringValue


//************************************
// Method:    GetUserProfileDir : Read in user profile directory
// FullName:  CAutoStartupInfo::GetUserProfileDir
// Access:    private 
// Returns:   bool
// Qualifier: const
// Parameter: CString & csUserProfileDir_o
// Parameter: CString & csAllUserProfileDir_o
//************************************
bool CAutoStartupInfo::GetUserProfileDir( CString& csUserProfileDir_o,
                                          CString& csAllUserProfileDir_o ) const
{ 
    HANDLE hProcToken = 0;
    if( !OpenProcessToken( GetCurrentProcess(), TOKEN_READ, &hProcToken ))
    {
        TRACE_ERR( "Failed to open process token for accessing user profile directory" );
        return false;
    }// End if

    // Auto closes handle when out of scope
    Utils::AutoHandleMgr ahmHndlMgr( hProcToken );

    // Read in user profile directory
    DWORD dwProfileDirBuffLen = MAX_PATH;
    const BOOL bCurrUserResult = ::GetUserProfileDirectory( hProcToken, 
                                                            csUserProfileDir_o.GetBufferSetLength( dwProfileDirBuffLen ),
                                                            &dwProfileDirBuffLen );
    csUserProfileDir_o.ReleaseBuffer();

    // Reset buffer length
    dwProfileDirBuffLen = MAX_PATH;

    // Read in all users profile directory
    const BOOL bAllUserResult = ::GetAllUsersProfileDirectory( csAllUserProfileDir_o.GetBufferSetLength( dwProfileDirBuffLen ),
                                                               &dwProfileDirBuffLen );
    csAllUserProfileDir_o.ReleaseBuffer();

    // Return status
    return ( bCurrUserResult != FALSE || bAllUserResult != FALSE );
}// End GetUserProfileDir


//************************************
// Method:    GetStartupFolderPath : Get startup folder which can be found in the Start Menu
//                                   directory of current user's profile directory
// FullName:  CAutoStartupInfo::GetStartupFolderPath
// Access:    private 
// Returns:   bool
// Qualifier: const
// Parameter: CString & csStartupFolderPath_o
// Parameter: CString & csAllUsersStartupFolderPath_o
//************************************
bool CAutoStartupInfo::GetStartupFolderPath( CString& csStartupFolderPath_o,
                                             CString& csAllUsersStartupFolderPath_o ) const
{
    // First get user profile directory
    if( !GetUserProfileDir( csStartupFolderPath_o,
                            csAllUsersStartupFolderPath_o ))
    {
        return false;
    }

    // Startup path to append
    LPCTSTR lpctszStartupPathToAppend = _T( "\\Start Menu\\Programs\\Startup\\" );

    // Append startup path
    csStartupFolderPath_o += lpctszStartupPathToAppend;
    csAllUsersStartupFolderPath_o += lpctszStartupPathToAppend;

    return true;
}// End GetStartupFolderPath


//************************************
// Method:    ReadInAutoStartAppsFromStartupFolder
// FullName:  CAutoStartupInfo::ReadInAutoStartAppsFromStartupFolder
// Access:    private 
// Returns:   bool
// Qualifier:
//************************************
bool CAutoStartupInfo::FindExeFromPath( CString csPath_i )
{
    // Prepare wild card string for fetching all files from the startup folder
    csPath_i += _T( "*.*" );

    // Helper for finding files
    CFileFind cfFileFinder;
    BOOL bFound = cfFileFinder.FindFile( csPath_i );

    // Keep searching
    while( bFound )
    {
        bFound = cfFileFinder.FindNextFile();

        // Skip dots and directories
        if( cfFileFinder.IsDots() || 
            cfFileFinder.IsDirectory() )
        {
            continue;
        }

        CString csFilePath = cfFileFinder.GetFilePath();
        if( !PathIsExe( csFilePath ))
        {
            if( Utils::IsShortcut( csFilePath ))
            {
                LPTSTR lptszPath = csFilePath.GetBufferSetLength( MAX_PATH );

                // Resolve path
                const BOOL bIsResolved = Utils::ResolveShortcut( lptszPath );
                csFilePath.ReleaseBuffer();
                if( !bIsResolved )
                {
                    continue;
                }
            }// End if
        }// End if

        // Found a valid exe, store it in our collection
        AddAutoStartExePath( csFilePath );
    }// End while

    return true;
}// End FindExeFromPath


//************************************
// Method:    ReadInAutoStartAppsFromStartupFolder
// FullName:  CAutoStartupInfo::ReadInAutoStartAppsFromStartupFolder
// Access:    private 
// Returns:   bool
// Qualifier:
//************************************
bool CAutoStartupInfo::ReadInAutoStartAppsFromStartupFolder()
{
    CString csCurrUserStartupFolderPath;
    CString csAllUsersStartupFolderPath;
    if( !GetStartupFolderPath( csCurrUserStartupFolderPath, csAllUsersStartupFolderPath ))
    {
        return false;
    }

    FindExeFromPath( csCurrUserStartupFolderPath );
    FindExeFromPath( csAllUsersStartupFolderPath );
    return true;
}