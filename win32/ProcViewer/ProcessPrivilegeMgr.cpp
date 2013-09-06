#include "stdafx.h"
#include "ProcessPrivilegeMgr.h"

CProcessPrivilegeMgr::CProcessPrivilegeMgr()
{}

CProcessPrivilegeMgr::~CProcessPrivilegeMgr()
{}

// Opens and keeps a process's token
bool CProcessPrivilegeMgr::OpenKeepProcessToken( const HANDLE hProcess_i )
{
	if( !hProcess_i )
	{
		ASSERT( FALSE );
		return false;
	}

	m_ahmProcessToken.Close();
	if( !OpenProcessToken( hProcess_i, TOKEN_ALL_ACCESS, &m_ahmProcessToken.m_hHandle ))
	{
		// Do your best to open the process
		if( !OpenProcessToken( hProcess_i, TOKEN_READ, &m_ahmProcessToken.m_hHandle ))
		{
			TRACE_ERR( "Failed to open process token" );
			return false;
		}
	}

	return true;
}

// Makes an entry for all privileges in the privilege map
void CProcessPrivilegeMgr::InitPrivilegeMap()
{
    // Set default size of privilege map
    GetPrivilegeMap().RemoveAll();
    GetPrivilegeMap().InitHashTable( 30, true );

    LPCTSTR* lpctszPrivilegeNames = GetPrivilegeNameArray();
    for( int nIndex = 0; lpctszPrivilegeNames[nIndex]; ++nIndex )
    {
        CPrivilege Privilege;
        if( !Privilege.GetPrivilegeDetails( lpctszPrivilegeNames[nIndex] ))
        {
            TRACE_ERR( "Failed to get privilege details" );
            continue;
        }

        // Add this privilege to map
        AddPrivilege( Privilege );
    }// End for

    // Load token privileges
    PTOKEN_PRIVILEGES pTokenPrivileges = LoadPrivileges();
    if( !pTokenPrivileges )
    {
        TRACE_ERR( "Failed to load privileges for given process!" );
        return;
    }

    // Auto delete
    Utils::AutoDeleter<TOKEN_PRIVILEGES, true> ahmHandleMgr( &pTokenPrivileges );

    // Loop through and get enable or disable status of a privilege
    for( DWORD dwIndex = 0; dwIndex < pTokenPrivileges->PrivilegeCount; ++dwIndex )
    {
        // Get name of a privilege
        DWORD dwBuffSize = MAX_PATH;
        TCHAR szBuff[MAX_PATH] = { 0 } ;
        if( !LookupPrivilegeName( NULL, 
                                  &pTokenPrivileges->Privileges[dwIndex].Luid, 
                                  szBuff, 
                                  &dwBuffSize ))
        {
            TRACE_ERR( "Privilege name lookup failed" );
            continue;
        }

        PrivilegeMap::CPair* pPair = GetPrivilegeMap().PLookup( szBuff );
        if( !pPair )
        {
            continue;
        }

        // Set enabled or disabled status for a privilege
        pPair->value.SetPrivilegeEnabled( IsPrivilegeEnabled( pTokenPrivileges->Privileges[dwIndex].Attributes ));
    }// End for
}// End InitPrivilegeMap

LPCTSTR* CProcessPrivilegeMgr::GetPrivilegeNameArray()
{
	    static LPCTSTR lpctszPrivileges[] = 
                                     { SE_CREATE_TOKEN_NAME,
									   SE_ASSIGNPRIMARYTOKEN_NAME,
                                       SE_LOCK_MEMORY_NAME,
                                       SE_INCREASE_QUOTA_NAME,
                                       SE_UNSOLICITED_INPUT_NAME,
                                       SE_MACHINE_ACCOUNT_NAME,
                                       SE_TCB_NAME,
                                       SE_SECURITY_NAME,
                                       SE_TAKE_OWNERSHIP_NAME,
                                       SE_LOAD_DRIVER_NAME,
                                       SE_SYSTEM_PROFILE_NAME,
                                       SE_SYSTEMTIME_NAME,
                                       SE_PROF_SINGLE_PROCESS_NAME,
                                       SE_INC_BASE_PRIORITY_NAME,
                                       SE_CREATE_PAGEFILE_NAME,
                                       SE_CREATE_PERMANENT_NAME,
                                       SE_BACKUP_NAME,
                                       SE_RESTORE_NAME,
                                       SE_SHUTDOWN_NAME,
                                       SE_DEBUG_NAME,
                                       SE_AUDIT_NAME,
                                       SE_SYSTEM_ENVIRONMENT_NAME,
                                       SE_CHANGE_NOTIFY_NAME,
                                       SE_REMOTE_SHUTDOWN_NAME,
                                       SE_UNDOCK_NAME,
                                       SE_SYNC_AGENT_NAME,          
                                       SE_ENABLE_DELEGATION_NAME,   
                                       SE_MANAGE_VOLUME_NAME,
                                       SE_IMPERSONATE_NAME,
                                       SE_CREATE_GLOBAL_NAME,
                                       0 };
        return lpctszPrivileges;
}// End GetPrivilegeNameArray


// Load privileges for processes
PTOKEN_PRIVILEGES CProcessPrivilegeMgr::LoadPrivileges()
{
    DWORD dwBufferSize = 0;
    GetTokenInformation( GetToken(), TokenPrivileges, NULL, 0, &dwBufferSize );
    if( !dwBufferSize )
    {
        TRACE_ERR( "Failed to get token information buffer size" );
        return NULL;
    }

    // Prepare buffer size
    PTOKEN_PRIVILEGES pTkPrivs = RCAST( PTOKEN_PRIVILEGES, new BYTE[dwBufferSize] );
    if( !pTkPrivs )
    {
        TRACE_ERR( "Failed to allocate memory for token privileges" );
        return NULL;
    }

    // Auto delete
    Utils::AutoDeleter<TOKEN_PRIVILEGES, true> adPrivDeletor( &pTkPrivs );

    // Retrieve information
    if( !GetTokenInformation( GetToken(), TokenPrivileges, pTkPrivs, dwBufferSize, &dwBufferSize ))
    {
        TRACE_ERR( "Failed to get token information" );
        return NULL;
    }

    return adPrivDeletor.Detach();
}// End LoadProcessPrivileges

// Enable/Disable a privilege
bool CProcessPrivilegeMgr::EnablePrivilege( LPCTSTR lpctszPrivilege_i, const bool Enable )
{
	if( !lpctszPrivilege_i )
	{
		return false;
	}

    PrivilegeMap::CPair* pPair = GetPrivilegeMap().PLookup( lpctszPrivilege_i );
    return pPair && pPair->value.EnablePrivilege( GetToken(), Enable );
}// End EnablePrivilege

// Enable/Disable all privileges
int CProcessPrivilegeMgr::EnableAllPrivileges( const bool Enable )
{
    // Get first
    PrivilegeMap::CPair* pPair = m_ProcessPrivileges.PGetFirstAssoc();

    // Return value of greater than zero means complete, less than zero means
    // some privileges were enabled, zero means all went well
    int nResult = 0;

    // Verify pointer
    if( !pPair )
    {
        nResult = 1;
    }

    // Loop through
    while( pPair )
    {
        // Enable/Disable privilege
        if( !pPair->value.EnablePrivilege( GetToken(), Enable ))
        {
            // Failed
            --nResult;
        }

        // Get next privilege entry from map
        pPair = m_ProcessPrivileges.PGetNextAssoc( pPair );
    }

    // Return result
    return nResult;
}// End EnableAllPrivileges

int CProcessPrivilegeMgr::RemoveAllPrivileges()
{
    // Get first
    PrivilegeMap::CPair* pPair = m_ProcessPrivileges.PGetFirstAssoc();

    // Return value of greater than zero means complete, less than zero means
    // some privileges were enabled, zero means all went well
    int nResult = 0;

    // Verify pointer
    if( !pPair )
    {
        nResult = 1;
    }

    // Loop through
    while( pPair )
    {
        // Enable/Disable privilege
        if( !pPair->value.RemovePrivilege( GetToken() ))
        {
            // Failed
            --nResult;
        }

        // Get next privilege entry from map
        pPair = m_ProcessPrivileges.PGetNextAssoc( pPair );
    }

    // Return result
    return nResult;
}// End RemoveAllPrivileges