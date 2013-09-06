// ***************************************************************
//  WindowCollection   version:  1.0   ·  date: 10/27/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Maintains a collection of windows for a process
// ***************************************************************

#include "stdafx.h"
#include "WindowCollection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//************************************
// Method:    WindowCollection -- Default constructor
// FullName:  WindowCollection::WindowCollection
// Access:    public 
// Returns:   
// Qualifier:
//************************************
WindowCollection::WindowCollection()
{
    // Initialize Hash Table for better performance
    m_WindowDataMap.InitHashTable( 53, FALSE );
}

//************************************
// Method:    ~WindowCollection -- Destructor
// FullName:  WindowCollection::~WindowCollection
// Access:    public 
// Returns:   
// Qualifier:
//************************************
WindowCollection::~WindowCollection()
{
    Clear();
}


//************************************
// Method:    Enumerate - Starts enumeration for windows
// FullName:  WindowCollection::Enumerate
// Access:    public 
// Returns:   bool
// Qualifier:
//************************************
bool WindowCollection::Enumerate()
{
    // Clear any previous information before proceeding
    Clear();
    return EnumWindows( EnumerateWindowsCB, reinterpret_cast<LPARAM>( this )) != FALSE;
}


//************************************
// Enumeration callback function. Invoked for every window found
//
// Method:    EnumerateWindowsCB -- Callback function for enumerating windows.
// FullName:  WindowCollection::EnumerateWindowsCB
// Access:    private static 
// Returns:   BOOL CALLBACK
// Qualifier:
// Parameter: HWND hWnd_i
// Parameter: LPARAM lParam_i
//************************************
BOOL CALLBACK WindowCollection::EnumerateWindowsCB( HWND hWnd_i, LPARAM lParam_i )
{
    // Get class pointer
    WindowCollection* pWndColl = reinterpret_cast<WindowCollection*>( lParam_i );
    if( !pWndColl )
    {
        return FALSE;
    }

    // Check if this process has already been fetched in
    DWORD dwProcessIdForWindow = 0;

    // Get thread id and process id for a window
    GetWindowThreadProcessId( hWnd_i, &dwProcessIdForWindow );

    PProcessWindowCollection pProcessWindowColl = 0;
    const BOOL bFound = pWndColl->m_WindowDataMap.Lookup( dwProcessIdForWindow, pProcessWindowColl );

    // If an entry for this process has not been made...
    if( !bFound )
    {
        // Then make one
        pProcessWindowColl = new ProcessWindowCollection( dwProcessIdForWindow );
        if( !pProcessWindowColl )
        {
            // Continue callback
            return TRUE;
        }

        // Store process window collection for this process
        pWndColl->m_WindowDataMap[dwProcessIdForWindow] = pProcessWindowColl;
    }
    else
    {
        // Else verify
        VERIFY( pProcessWindowColl->GetProcessID() == dwProcessIdForWindow );
    }// End if

    // Load all process windows for this process
    pProcessWindowColl->LoadProcessWindow( hWnd_i );

    // We want more, We want more, We want more .... ;)
    return TRUE;
}// End EnumerateCB


//************************************
// Method:    Clear -- Clears internal window map
// FullName:  WindowCollection::Clear
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void WindowCollection::Clear()
{
    POSITION pstPos = m_WindowDataMap.GetStartPosition();
    while( pstPos )
    {
        DWORD dwProcessID = 0;
        PProcessWindowCollection pProcessWindowColl = 0;

        m_WindowDataMap.GetNextAssoc( pstPos, dwProcessID, pProcessWindowColl );
        if( pProcessWindowColl )
        {
            delete pProcessWindowColl;
            pProcessWindowColl  = 0;
        }
    }// End while

    m_WindowDataMap.RemoveAll();
}// End Clear


//************************************
// Method:    GetProcessWindowCollection - Returns process collection pointer for given process id
// FullName:  WindowCollection::GetProcessWindowCollection
// Access:    public 
// Returns:   const PProcessWindowCollection
// Qualifier: const
// Parameter: const DWORD dwProcessID_i
//************************************
const PProcessWindowCollection WindowCollection::GetProcessWindowCollection( const DWORD dwProcessID_i ) const
{
    PProcessWindowCollection pProcessWindowColl = 0;
    m_WindowDataMap.Lookup( dwProcessID_i, pProcessWindowColl );
    
    return pProcessWindowColl;
}