#include "stdafx.h"
#include "Privilege.h"

CPrivilege::CPrivilege()
{}

CPrivilege::~CPrivilege()
{}

CPrivilege::CPrivilege( const CPrivilege& PrivilegeToCopy )
   :  m_csPrivilegeName( PrivilegeToCopy.GetPrivilegeName() ),
      m_csPrivilegeDescription( PrivilegeToCopy.GetPrivilegeDescription() ),
      m_PrivilegeEnabled( PrivilegeToCopy.GetPrivilegeEnabled() )
{}

const CPrivilege& CPrivilege::operator= ( const CPrivilege& PrivilegeToAssign )
{
   if( this != &PrivilegeToAssign )
   {
      m_csPrivilegeName         = PrivilegeToAssign.GetPrivilegeName();
      m_csPrivilegeDescription  = PrivilegeToAssign.GetPrivilegeDescription();
      m_PrivilegeEnabled        = PrivilegeToAssign.GetPrivilegeEnabled();
   }

   return *this;
}

bool CPrivilege::GetPrivilegeDetails( LPCTSTR lpctszPrivilegeName_i )
{
    // Privilege instance to be stored in map
    SetPrivilegeName( lpctszPrivilegeName_i );
    SetPrivilegeEnabled( false );

    // Get the friendly display name for a privilege
    DWORD dwBuffSize            = MAX_PATH;
    TCHAR szPrivBuff[MAX_PATH]  = { 0 };
    DWORD dwLangId = 0;
    if( !LookupPrivilegeDisplayName( NULL, GetPrivilegeName(), szPrivBuff, &dwBuffSize, &dwLangId ))
    {
        TRACE_ERR( "Failed to get display name for privilege" );
        return false;
    }

    // Set description of privilege too
    SetPrivilegeDescription( szPrivBuff );
    return true;
}// End GetPrivilegeDetails

DWORD CPrivilege::SetPrivilegeAttributes( const HANDLE hToken_i, DWORD dwAttributes_i )
{
    if( !hToken_i )
    {
        return false;
    }

    // First get LUID for privilege name
    TOKEN_PRIVILEGES tkpPriv = { 0 };
    if( !LookupPrivilegeValue( NULL, 
                               GetPrivilegeName(), 
                               &tkpPriv.Privileges[0].Luid ))
    {
        return false;
    }

    // Now enable or disable privilege
    tkpPriv.PrivilegeCount = 1;
    tkpPriv.Privileges[0].Attributes = dwAttributes_i;
    AdjustTokenPrivileges( hToken_i, FALSE, &tkpPriv, 0, 0, 0 );

    return GetLastError();
}

bool CPrivilege::EnablePrivilege( const HANDLE hToken_i, const bool Enable_i )
{
    const bool bStatus = ( SetPrivilegeAttributes( hToken_i, Enable_i ? SE_PRIVILEGE_ENABLED : 0 ) == ERROR_SUCCESS );
    if( bStatus )
    {
        SetPrivilegeEnabled( Enable_i );
    }

    return bStatus;
}// End EnablePrivilege

bool CPrivilege::RemovePrivilege( const HANDLE hToken_i )
{
    const bool bStatus = ( SetPrivilegeAttributes( hToken_i, SE_PRIVILEGE_REMOVED ) == ERROR_SUCCESS );
    if( bStatus )
    {
        SetPrivilegeEnabled( false );
    }

    return bStatus;
}