#ifndef _API_MGR_H_
#define _API_MGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "TypedefsInclude.h"

class APIMgr  
{
    public:
	    APIMgr();
	    virtual ~APIMgr();

        // Loads and free's any modules
        HMODULE LoadModule( LPCTSTR lpctszModuleName_i );
        void FreeModules();

        // Return procedure address of a procedure
        FARPROC GetProcAddress( LPCSTR lpcszProcName_i, LPCTSTR lpctszModuleName_i );

        //************************************
        // Method:    FindProcPtrByName
        // FullName:  APIMgr::FindProcPtrByName
        // Access:    public 
        // Returns:   FARPROC
        // Qualifier:
        // Parameter: LPCTSTR lpctszProcName_i
        //************************************
        FARPROC FindProcPtrByName( LPCTSTR lpctszProcName_i )
        {
            // First search whether we have this function pointer with us
            FARPROC ProcAddr = 0;
            m_FuncPtrMap.Lookup( lpctszProcName_i, ProcAddr );

            return ProcAddr;
        }

        //************************************
        // Method:    FindModuleByName
        // FullName:  APIMgr::FindModuleByName
        // Access:    public 
        // Returns:   HMODULE
        // Qualifier:
        // Parameter: LPCTSTR lpctszModuleName_i
        //************************************
        HMODULE FindModuleByName( LPCTSTR lpctszModuleName_i )
        {
            // Get module pointer from map if there is any
            HMODULE hMod = 0;
            m_ModuleHandleMap.Lookup( lpctszModuleName_i, hMod );
            return hMod;
        }

    private:

        // Map variables
        typedef CMap<CString, LPCTSTR, HMODULE, HMODULE&> ModuleHandleMap;
        typedef CMap<CString, LPCTSTR, FARPROC, FARPROC&> ModuleProcMap;

        ModuleProcMap m_FuncPtrMap;
        ModuleHandleMap m_ModuleHandleMap;
};// End APIMgr

extern APIMgr g_APIMgr;

#endif // _API_MGR_H_
