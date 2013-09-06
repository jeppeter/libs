/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * ProcessSymbolCollection.cpp - Implementation file for ProcessSymbolCollection.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-19
 */


#include "stdafx.h"
#include "ProcessSymbolCollection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Static variables //

CString ProcessSymbolCollection::m_csSymbolPath;
// Initialize env var status
bool ProcessSymbolCollection::m_bNTSymbolPathEnvVarDefined = ProcessSymbolCollection::GetValidSymbolPath();

/** 
 * 
 * Default constructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ProcessSymbolCollection::ProcessSymbolCollection() : m_hProcess( 0 ), m_bInitialized( false )
{
    m_ModSymMap.InitHashTable( 53, FALSE );
}


/** 
 * 
 * Destructor.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ProcessSymbolCollection::~ProcessSymbolCollection()
{
    // Clear list
    Clear();
}


/** 
 * 
 * Clears symbol list
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void ProcessSymbolCollection::Clear()
{
    if( m_bInitialized && m_hProcess )
    {
        VERIFY( SymCleanup( m_hProcess ));
        m_bInitialized = false;
        m_hProcess = 0;
    }// End if

    POSITION pstPos = m_ModSymMap.GetStartPosition();
    while( pstPos )
    {
        // Delete entries from map
        SymbolCollection* pSymData = 0;
        DWORD dwKey = 0;
        m_ModSymMap.GetNextAssoc( pstPos, dwKey, pSymData );

        delete pSymData;
        pSymData = 0;
    }

    // Clear entries
    m_ModSymMap.RemoveAll();
    m_bInitialized = false;
    m_hProcess = 0;
    m_csProcessName.Empty();
}// End clear


/** 
 * 
 * Initialization routine.
 * 
 * @param       hProcess_i          - Handle to process.
 * @param       lpctszProcessName_i - Name of the process.
 * @return      bool                - Return execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool ProcessSymbolCollection::Init( HANDLE hProcess_i, 
                                    LPCTSTR lpctszProcessName_i )
{
    // Initially set flag to false
    m_bInitialized = false;

    // Process initialization
    if( !hProcess_i )
    {
        ASSERT( hProcess_i );
        return false;
    }

	// Set handle
	m_hProcess = hProcess_i;

    // Set options
    SymSetOptions( SYMOPT_DEFERRED_LOADS | 
                   SYMOPT_LOAD_ANYTHING | 
                   SYMOPT_ALLOW_ABSOLUTE_SYMBOLS | 
				   SYMOPT_UNDNAME );

    // Initialize symbols for this process
    USES_CONVERSION;
#ifdef _UNICODE
    if( !SymInitialize( m_hProcess,(PCWSTR) T2A( GetSymbolPath() ), TRUE ))
    {
        return false;
    }
#else
    if( !SymInitialize( m_hProcess, T2A( GetSymbolPath() ), TRUE ))
    {
        return false;
    }
#endif
    m_bInitialized = true;
    m_csProcessName = lpctszProcessName_i;

    // Return success
    return true;
}// End init


/** 
 * 
 * Enumerates one module in a process
 * 
 * @param       dwDllBases_i - Base address of module to enumerate
 * @return      bool         - Return true
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool ProcessSymbolCollection::EnumerateModuleSymbols( DWORD dwDllBases_i, 
                                                      SymbolCollection*& pModSymColl_o,
                                                      LPCTSTR lpctszModuleFileName_i,
                                                      const DWORD dwModuleSize_i,
                                                      const bool bRefresh_i )
{
    UNREFERENCED_PARAMETER( lpctszModuleFileName_i );
    UNREFERENCED_PARAMETER( dwModuleSize_i );

    bool bFound = false;
    if( bRefresh_i )
    {
        // Find in map
        bFound = m_ModSymMap.Lookup( dwDllBases_i, pModSymColl_o ) != FALSE;

        if( bFound )
        {
            // Remove entry from map
            m_ModSymMap.RemoveKey( dwDllBases_i );
            delete pModSymColl_o;
        }// End if
    }// End if

    if( bRefresh_i || !m_ModSymMap.Lookup( dwDllBases_i, pModSymColl_o ))
    {
        // Allocate a new module symbol collection
        pModSymColl_o = new SymbolCollection;
    	SymEnumSymbols( m_hProcess, 
						dwDllBases_i, 
						0, 
						EnumModuleSymbolsCB,  
						pModSymColl_o );

        // Add this entry to map
        m_ModSymMap[dwDllBases_i] = pModSymColl_o;
    }// End if

    return true;
}// End Enumerate


/** 
 * 
 * Enumerate through list of dlls given for symbols
 * 
 * @param       lpdwDllBases_i - Array of base of dlls
 * @param       dwCount_i      - Returns count of dlls
 * @return      bool           - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool ProcessSymbolCollection::EnumerateModuleSymbols( LPDWORD lpdwDllBases_i, 
                                                      const DWORD dwCount_i,
                                                      LPCTSTR lpctszModuleFileName_i,
                                                      const DWORD dwModuleSize_i )
{
    // Check if we have initialized properly
    if( !m_bInitialized || !m_hProcess )
    {
        ASSERT( false );
        return false;
    }

    // Loop through and enumerate all modules
    for( DWORD dwIndex = 0; dwIndex < dwCount_i; ++dwIndex )
    {
        // Symbol collection
        SymbolCollection* pModSymColl = 0;

        // Enumerate through each item
        VERIFY( EnumerateModuleSymbols( lpdwDllBases_i[dwIndex], 
                                        pModSymColl, 
                                        lpctszModuleFileName_i, 
                                        dwModuleSize_i, 
                                        false ));
    }// End for

    // Return execution status
    return true;
}// End Enumerate


/** 
 * 
 * Callback function for enumeration of symbols in a loaded dll of a process
 * 
 * @param       lpszSymbolName_i  - Name of the symbol
 * @param       ulSymbolAddress_i - Address of the symbol
 * @param       ulSymbolSize_i    - Size of the symbol
 * @param       lpvdUserContext_i - Custom object passed in from caller
 * @return      BOOL              - If true then we get more callbacks
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */

BOOL ProcessSymbolCollection::EnumModuleSymbolsCB( PSYMBOL_INFO pSymInfo_i,
												   ULONG ulSymbolSize_i,
												   LPVOID lpvdUserContext_i )
{
	UNREFERENCED_PARAMETER( ulSymbolSize_i );
    SymbolCollection* pModSymColl = RCAST( SymbolCollection*, lpvdUserContext_i );
    if( !pModSymColl )
    {
        // Ignore this error and get more
        return TRUE;
    }

	PSymbolData pSymbolData = new SymbolData;
	if( !pSymbolData )
	{
		return TRUE;
	}

	pSymbolData->SetSymbolData( *pSymInfo_i );

    // Add symbol to list
    pModSymColl->AddSymbol( pSymbolData );

    // We want more
    return TRUE;

}// End EnumSymbolsCB
