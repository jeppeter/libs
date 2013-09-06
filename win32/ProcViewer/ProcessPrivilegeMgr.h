#ifndef _PROCESS_PRIVILEGE_MGR_H_
#define _PROCESS_PRIVILEGE_MGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Privilege.h"
#include "TypedefsInclude.h"

typedef enum
{
    Ps_Success      = 0,
    Ps_Error        = 1,
    Ps_SomeFailed   = 2,
}PrivilegeStatus;

class CProcessPrivilegeMgr  
{
    public:
		// Constructor
        CProcessPrivilegeMgr();

		// Destructor
        virtual ~CProcessPrivilegeMgr();

		bool OpenKeepProcessToken( const HANDLE hProcess_i );
		HANDLE GetToken() const { return m_ahmProcessToken; }
    
        PTOKEN_PRIVILEGES LoadPrivileges();

        int EnableAllPrivileges( const bool Enable );
        int RemoveAllPrivileges();
        bool EnablePrivilege( LPCTSTR lpctszPrivilege_i, const bool Enable );

        bool IsPrivilegeEnabled( LPCTSTR lpctszPrivName_i ) const
        {
            const PrivilegeMap::CPair* pPair = GetPrivilegeMap().PLookup( lpctszPrivName_i );
            return pPair && pPair->value.GetPrivilegeEnabled();
        }

        const PrivilegeMap& GetPrivilegeMap() const
        {
            return m_ProcessPrivileges;
        }
        PrivilegeMap& GetPrivilegeMap()
        {
            return m_ProcessPrivileges;
        }

        void InitPrivilegeMap();

    protected:

		LPCTSTR* GetPrivilegeNameArray();

        // Returns true if privilege given is enabled
        bool IsPrivilegeEnabled( const DWORD dwAttributes_i ) const
        {
            return ( Utils::IsValidMask( dwAttributes_i, SE_PRIVILEGE_ENABLED ) ||
                     Utils::IsValidMask( dwAttributes_i, SE_PRIVILEGE_ENABLED_BY_DEFAULT ) ||
                     Utils::IsValidMask( dwAttributes_i, SE_PRIVILEGE_USED_FOR_ACCESS ));
        }

        void AddPrivilege( const CPrivilege& Privilege )
        {
            GetPrivilegeMap()[Privilege.GetPrivilegeName()] = Privilege;
        }
    
    private:

        PrivilegeMap m_ProcessPrivileges;
		Utils::AutoHandleMgr m_ahmProcessToken;
};// End CProcessPrivilegeMgr

#endif // _PROCESS_PRIVILEGE_MGR_H_