#ifndef _PRIVILEGE_H_
#define _PRIVILEGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPrivilege  
{
   public:
       CPrivilege();
       CPrivilege( const CPrivilege& PrivilegeToCopy );
       const CPrivilege& operator= ( const CPrivilege& PrivilegeToAssign );
       virtual ~CPrivilege();

       bool GetPrivilegeDetails( LPCTSTR lpctszPrivilegeName_i );
       
       // Setter and getter for privilege
       void SetPrivilegeName( LPCTSTR lpctszPrivName_i )
       {
           m_csPrivilegeName = lpctszPrivName_i;
       }
       const CString& GetPrivilegeName() const 
       { 
           return m_csPrivilegeName; 
       }
       
       // Setter and getter for privilege description
       void SetPrivilegeDescription( LPCTSTR lpctszPrivDescription_i )
       {
           m_csPrivilegeDescription = lpctszPrivDescription_i;
       }
       const CString& GetPrivilegeDescription() const
       {
           return m_csPrivilegeDescription;
       }
       
       // Setter and getter privilege enabled status
       void SetPrivilegeEnabled( const bool PrivilegeEnabled_i )
       {
           m_PrivilegeEnabled = PrivilegeEnabled_i;
       }
       bool GetPrivilegeEnabled() const 
       { 
           return m_PrivilegeEnabled; 
       }

   protected:

       DWORD SetPrivilegeAttributes( const HANDLE hToken_i, const DWORD uAttribute_i );
       bool EnablePrivilege( const HANDLE hToken_i, const bool Enable_i );
       bool RemovePrivilege( const HANDLE hToken_i );

   private:

	   // Privilege mgr
	   friend class CProcessPrivilegeMgr;

       CString m_csPrivilegeName;
       CString m_csPrivilegeDescription;
       bool    m_PrivilegeEnabled;
};

#endif // _PRIVILEGE_H_