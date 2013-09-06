#include "stdafx.h"
#include "window.h"

#define MAKE_STR( ToStr ) _T( #ToStr )

// Initialize static variable
StyleParser Window::m_StyleParser;

// Clears window data list
void ClearWindowDataList( WindowList& wlWindowList )
{
    // Clear data
    POSITION pstPos = wlWindowList.GetHeadPosition();
    while( pstPos )
    {
        PWindow pWindow = wlWindowList.GetNext( pstPos );
        delete pWindow;
    }

    // Remove all items from list
    wlWindowList.RemoveAll();
}

//************************************
// Method:    RetrieveWndStyles
// FullName:  Window::RetrieveWndStyles
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************
void Window::RetrieveWndStyles()
{
    // Parse out class style and store it in string
    GetStyleParser().GetClassStyleAsString( GetClass().style, GetClassStyleString() );

    // Ge normal window style
    GetStyleParser().GetWindowStyleAsString( GetStyle(), GetWindowStyleString() );

    // Get control specific style if any
    GetStyleParser().GetStyleStringForWindowByClassName( GetStyle(), 
                                                         GetClassName(),
                                                         GetWindowStyleString() );

    // Get default extended style for a window
    GetStyleParser().GetWindowStyleExAsString( GetStyleEx(), GetWindowStyleExString() );

    // Style extended, do special handling for list control
    DWORD dwStyleEx = 0;

    // Specialized extended style handling for some controls like
    // list view, comboex, tab controls etc
    if( IsListControl() )
    {
        Utils::SndMsgTimeOutHelper( GetHandle(), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0, dwStyleEx );
    }
    else if( IsComboExControl() )
    {
        Utils::SndMsgTimeOutHelper( GetHandle(), CBEM_GETEXTENDEDSTYLE, 0, 0, dwStyleEx );
    }
    else if( IsTabControl() )
    {
        Utils::SndMsgTimeOutHelper( GetHandle(), TCM_GETEXTENDEDSTYLE, 0, 0, dwStyleEx );
    }
    else
    {
        dwStyleEx = GetStyleEx();
    }// End if

    // Parse out window style ex and store it in string
    GetStyleParser().GetStyleExStringForWindowByClassName( dwStyleEx, 
                                                           GetClassName(),
                                                           GetWindowStyleExString() );
}// End RetrieveWndStyles

/** 
 * 
 * Extracts details pertaining to a window
 * 
 * @param       WindowObj - Window data
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Window::ExtractWindowDetails( const HWND hWnd_i )
{
    // Set window handle
    SetHandle( hWnd_i );

    // Set thread id of window
    SetThreadId( ::GetWindowThreadProcessId( GetHandle(), 0 ));

    // Get class name of window
    TCHAR szBuffer[MAX_PATH] = { 0 };
    ::GetClassName( GetHandle(), szBuffer, MAX_PATH );
    GetClassName() = szBuffer;

    GetClass().cbSize = sizeof( GetClass() );
    GetClassInfoEx( AfxGetInstanceHandle(), szBuffer, &GetClass() );

    // Get window text if any
    InternalGetWindowText( GetHandle(), GetTitle().GetBufferSetLength( MAX_PATH ), MAX_PATH );
    GetTitle().ReleaseBuffer();

    // Get normal style
    SetStyle( GetWindowLong( GetHandle(), GWL_STYLE ));

    // Get extended style
    SetStyleEx( GetWindowLong( GetHandle(), GWL_EXSTYLE ));

    // Get window id
    SetId( GetWindowLong( GetHandle(), GWL_ID ));

    // Get parent window
    SetParentHandle( RCAST( HWND, GetWindowLong( GetHandle(), GWL_HWNDPARENT )));

    // Window state i.e. window is maximized, minimized or restored
    GetStateAsString( GetState() );

    // For style parsing
    RetrieveWndStyles();

    // Window bounds
    CRect crBounds;
    GetWindowRect( GetHandle(), &crBounds );
    if( crBounds.Width() || crBounds.Height() )
    {
        GetBounds().Format( _T( "L:%d T:%d R:%d B:%d" ), crBounds.left, 
                                                         crBounds.top, 
                                                         crBounds.right, 
                                                         crBounds.bottom );
    }// End if

    // Retrieves unicode support status for windows
    SetUnicode( IsWindowUnicode( GetHandle() ));

    // Get window icon
    DWORD dwResult = 0;
    Utils::SndMsgTimeOutHelper( GetHandle(), 
                                WM_GETICON, 
                                ICON_SMALL, 
                                0,
                                dwResult );
    // Get window icon
    SetIcon( RCAST( HICON, dwResult ));

    // Get enabled status of window
    SetEnabled( IsWindowEnabled( GetHandle() ));
}// End ExtractWindowDetails


//************************************
// Method:    GetStateAsString
// FullName:  Window::GetStateAsString
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: CString & csState_o
//************************************
void Window::GetStateAsString( CString& csState_o ) const
{
    // Get state of window
    if( ::IsZoomed( GetHandle() ))
    {
        // Window is in a maximized state
        csState_o = _T( "Maximized" );
    }
    else if( ::IsIconic( GetHandle() ))
    {
        // Window is in a minimized state
        csState_o = _T( "Minimized" );
    }
    else if( ::IsWindowVisible( GetHandle() ))
    {
        // State of window is normal
        csState_o = _T( "Restored" );
    }
    else
    {
        // Must be an invisible window
        csState_o = _T( "Invisible" );
    }// End if

}// End GetStateAsString

void Window::ParseOutStyles( ULONG ulStyle_i, 
                             CString& csStyle_o, 
                             const PFN_STRING_FOR_STYLE pfnStyleParser_i ) const
{
    ULONG ulMasker = 1;
    while( ulStyle_i )
    {
        const ULONG ulCurrentStyle = ulMasker & ulStyle_i;
        if( ulCurrentStyle ) 
        {
            ( this->*pfnStyleParser_i )( ulCurrentStyle, csStyle_o );
        }

        ++ulMasker;

        if( ulCurrentStyle && ulStyle_i )
        {
            csStyle_o += _T( " | " );
        }
    }// End while
}