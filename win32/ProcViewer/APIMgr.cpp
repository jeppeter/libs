#include "stdafx.h"
#include "APIMgr.h"

APIMgr g_APIMgr;

//************************************
// Method:    APIMgr
// FullName:  APIMgr::APIMgr
// Access:    public 
// Returns:   
// Qualifier:
//************************************
APIMgr::APIMgr()
{}

//************************************
// Method:    ~APIMgr
// FullName:  APIMgr::~APIMgr
// Access:    public 
// Returns:   
// Qualifier:
//************************************
APIMgr::~APIMgr()
{
    // Clear module list
    FreeModules();
}


//************************************
// Method:    LoadModule
// FullName:  APIMgr::LoadModule
// Access:    public 
// Returns:   HMODULE
// Qualifier:
// Parameter: LPCTSTR lpctszModuleName_i
//************************************
HMODULE APIMgr::LoadModule( LPCTSTR lpctszModuleName_i )
{
    HMODULE hMod = LoadLibrary( lpctszModuleName_i );
    if( !hMod )
    {
        return 0;
    }

    // Store module
    m_ModuleHandleMap[lpctszModuleName_i] = hMod;
    return hMod;
}


//************************************
// Method:    FreeModules
// FullName:  APIMgr::FreeModules
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void APIMgr::FreeModules()
{
    POSITION Pos = m_ModuleHandleMap.GetStartPosition();
    while( Pos )
    {
        // Key, Value vars
        CString csKey;
        HMODULE hMod = 0;

        // Get module at position and free
        m_ModuleHandleMap.GetNextAssoc( Pos, csKey, hMod );
        if( hMod )
        {
            FreeLibrary( hMod );
        }
    }// End while

    // Clear module and function pointer map
    m_ModuleHandleMap.RemoveAll();
    m_FuncPtrMap.RemoveAll();
}// End FreeModules


//************************************
// Method:    GetProcAddress
// FullName:  APIMgr::GetProcAddress
// Access:    public 
// Returns:   FARPROC
// Qualifier:
// Parameter: LPCSTR lpcszProcName_i
//************************************
FARPROC APIMgr::GetProcAddress( LPCSTR lpcszProcName_i, LPCTSTR lpctszModuleName_i )
{
    USES_CONVERSION;

    // Convert to unicode format
    LPCTSTR lpctszProcName = A2T(lpcszProcName_i);

    FARPROC ProcAddr = FindProcPtrByName( lpctszProcName );
    if( ProcAddr )
    {
        return ProcAddr;
    }
 
    HMODULE hMod = 0;
    if( m_ModuleHandleMap.Lookup( lpctszModuleName_i, hMod ) == FALSE )
    {
        hMod = LoadModule( lpctszModuleName_i );

        // Try loading the module
        if( !hMod )
        {
            return 0;
        }
    }// End if

    ProcAddr = ::GetProcAddress( hMod, lpcszProcName_i );
    if( !ProcAddr )
    {
        return 0;
    }

    // Store procedure address
    m_FuncPtrMap[lpctszProcName] = ProcAddr;

    return ProcAddr;
}// End GetProcAddress