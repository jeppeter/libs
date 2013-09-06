#include "stdafx.h"
#include "ProcessWindowCollection.h"

//************************************
// Method:    ProcessWindowCollection
// FullName:  ProcessWindowCollection::ProcessWindowCollection
// Access:    private 
// Returns:   
// Qualifier: : m_dwProcessId( 0 )
//************************************
ProcessWindowCollection::ProcessWindowCollection( const DWORD dwProcessId_i ) 
    :   m_dwProcessId( dwProcessId_i )
{}


//************************************
// Method:    ~ProcessWindowCollection
// FullName:  ProcessWindowCollection::~ProcessWindowCollection
// Access:    public 
// Returns:   
// Qualifier:
//************************************
ProcessWindowCollection::~ProcessWindowCollection()
{
    Clear();
}


//************************************
// Method:    Clear
// FullName:  ProcessWindowCollection::Clear
// Access:    public 
// Returns:   void
// Qualifier: Clears internal process window map
//************************************
void ProcessWindowCollection::Clear()
{
    ClearWindowDataList( m_wlProcessWindowList );
    m_wmWindowMap.RemoveAll();
}


//************************************
// Method:    LoadProcessWindow
// FullName:  ProcessWindowCollection::LoadProcessWindow
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: HWND hNewWindow_i
// Parameter: const DWORD dwWindowThreadId_i
//************************************
bool ProcessWindowCollection::LoadProcessWindow( HWND hNewWindow_i )
{
    // Prepare window data
    PWindow pWindow = new Window;

    // Window details
    pWindow->ExtractWindowDetails( hNewWindow_i );

    // Get parent child map
    WindowMap& wmWndMap = GetWindowMap();

    PWindow pParentWindow = 0;
    if( wmWndMap.Lookup( pWindow->GetParentHandle(), pParentWindow ) && pParentWindow )
    {
        // Add this window to the list of child windows of parent window
        pParentWindow->GetChildWindowList().AddTail( pWindow );
    }
    else
    {
        // Add to main window list
        GetProcessWindowList().AddTail( pWindow );
    }// End if

    // Insert item to map
    wmWndMap[pWindow->GetHandle()] = pWindow;

    // Get all child windows for this window
    EnumChildWindows( hNewWindow_i, 
					  EnumerateChildWindowsCB, 
					  reinterpret_cast<LPARAM>( &wmWndMap ));

    // We want more
    return TRUE;
}


/** 
 * 
 * Enumerates all child windows for a window.
 * 
 * @param       hWnd_i   - Child window
 * @param       lParam_i - Custom parameter passed in by caller
 * @return      BOOL     - Returns true if we need more child windows
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL CALLBACK ProcessWindowCollection::EnumerateChildWindowsCB( HWND hWnd_i, LPARAM lParam_i )
{
    // Return if windows is invalid
    if( !hWnd_i )
    {
        // We want more
        return TRUE;
    }

    // Get window data from LPARAM
    WindowMap& wmWindowMap = *reinterpret_cast< WindowMap* >( lParam_i );

    // Allocate memory for new child window data
    PWindow pChildWindow = new Window;

    // Get child window details
    pChildWindow->ExtractWindowDetails( hWnd_i );

    // Get parent window for this child window, should be in the map
    PWindow pParentWindow = 0;
    if( !wmWindowMap.Lookup( pChildWindow->GetParentHandle(), pParentWindow ) && !pParentWindow )
    {
        // Some trouble, there should be a parent window
        ASSERT( FALSE );

        // Anyway continue and try to get more
        return TRUE;
    }

    // Add this window to parent window's children list
    pParentWindow->GetChildWindowList().AddTail( pChildWindow );

    // Insert child window into map
    wmWindowMap[pChildWindow->GetHandle()] = pChildWindow;

    // We want more
    return TRUE;
}// End EnumerateChildWindowsCB