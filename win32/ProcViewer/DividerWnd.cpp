/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DividerWnd.cpp - Implementation file for divider window.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-01
 */

#include "stdafx.h"
#include "DeferWindowPosMgr.h"
#include "DividerWnd.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP( SizingBar, CWnd )
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// Register divider class only once hence we are using an extern variable ;)
const ATOM g_nRegisterDividerWndInvoker = DividerWnd::RegisterDividerWnd();

// Register sizing bar class only once hence we are using an extern variable ;)
const ATOM g_nRegisterSizingBarInvoker = SizingBar::RegisterSizingBar();

// Register toggler just once hence we are using extern variable here which only gets initialized once
const ATOM g_nRegisterToggler = Toggler::RegisterToggler();

// Prepare a registered window message to be send by sizing bar to divider window
const UINT WM_REGISTERED_DRAGMSG = RegisterWindowMessage( _T( "Sizing bar drag message - For God So Loved The World That He Sent His Only Begotten Son that Whosoever believeth in him should not perish but shall have everlasting life -- John 3:16" ));


// Map sizing message
BEGIN_MESSAGE_MAP( Toggler, CWnd )
    ON_WM_SIZE()
END_MESSAGE_MAP()


/** 
 * 
 * Default constructor.
 * 
 * @param       eDirection_i : Sizing bar direction.
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Toggler::Toggler( const DIRECTION_e eDirection_i ) : m_nPaneSpanBeforeToggling( 0 ), 
                                                     m_nTogglerSpan( 0 ), 
                                                     m_eDirection( eDirection_i )
{}


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
Toggler::~Toggler()
{}


/** 
 * 
 * TODO: Add Function Description Here.
 * 
 * @param       uType_i - 
 * @param       nCx_i   - 
 * @param       nCy_i   - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Toggler::OnSize( UINT uType_i, int nCx_i, int nCy_i )
{
    CWnd::OnSize( uType_i, nCx_i, nCy_i );
}


/** 
 * 
 * Registers toggler.
 * 
 * @param       Nil
 * @return      ATOM - Registration atom
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ATOM Toggler::RegisterToggler()
{
    // Window class
    WNDCLASSEX wcClassRegEx     = { 0 };
    wcClassRegEx.cbSize         = sizeof( wcClassRegEx );
    wcClassRegEx.hbrBackground  = ::GetSysColorBrush( COLOR_BTNFACE );
    wcClassRegEx.hCursor        = LoadCursor( 0, IDC_ARROW );
    wcClassRegEx.lpfnWndProc    = ::DefWindowProc;
    wcClassRegEx.lpszClassName  = TOGGLER_CLASS;
    wcClassRegEx.style          = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    // Register class
    return ::RegisterClassEx( &wcClassRegEx );
}


/** 
 * 
 * Creates toggler
 * 
 * @param       wndParent_i     - Window parent
 * @return      BOOL            - Returns creation status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL Toggler::Create( CWnd& wndParent_i)
{
    // Prepare toggler rectangle
    CRect crTogglerRect;
    wndParent_i.GetClientRect( &crTogglerRect );
    return CWnd::Create( TOGGLER_CLASS, 0, WS_CHILD | WS_VISIBLE, crTogglerRect, &wndParent_i, 12301, 0 );
}


/** 
 * 
 * Default constructor. An optional parameter for direction is given.
 * 
 * @param       eDirection_i - Sizing bar direction.
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
SizingBar::SizingBar( const DIRECTION_e eDirection_i ) : 
        m_eDirection( eDirection_i ),
        m_hCursor( 0 ),
        m_bIsSizable( true ),
        m_tgSPToggler( eDirection_i ),
        m_tgFPToggler( eDirection_i ),
        m_bIsSizing( false )
{}



/** 
 * 
 * Destructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
SizingBar::~SizingBar()
{}


/** 
 * 
 * Register sizing bar.
 * 
 * @param       Nil
 * @return      ATOM - Returns registration status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ATOM SizingBar::RegisterSizingBar()
{
    // Window class
    WNDCLASSEX wcClassRegEx     = { 0 };
    wcClassRegEx.cbSize         = sizeof( wcClassRegEx );
    wcClassRegEx.hbrBackground  = ::GetSysColorBrush( COLOR_BTNFACE );
    wcClassRegEx.hCursor        = LoadCursor( 0, IDC_SIZEWE );;
    wcClassRegEx.lpfnWndProc    = ::DefWindowProc;
    wcClassRegEx.lpszClassName  = SIZING_BAR_CLASS;
    wcClassRegEx.style          = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    // Register class
    return ::RegisterClassEx( &wcClassRegEx );
}// End RegisterSizingBar



/** 
 * 
 * Creates sizing bar. Also creates togglers along with it.
 * 
 * @param       wndParent_i - Parent window
 * @param       dwStyle_i   - Style
 * @param       dwStyleEx_i - Style extended.
 * @return      BOOL        - Returns creation status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL SizingBar::Create( CWnd& wndParent_i, 
                        const DWORD dwStyle_i, 
                        const DWORD dwStyleEx_i )
{
    if( CWnd::CreateEx( dwStyleEx_i, 
                        SIZING_BAR_CLASS, 
                        0, 
                        dwStyle_i, 
                        CRect( 0, 0, 0, 0 ), 
                        &wndParent_i,
                        10101,
                        0 ))
    {
        // Destroy if already created
        m_tgFPToggler.GetSafeHwnd() && m_tgFPToggler.DestroyWindow();
        m_tgSPToggler.GetSafeHwnd() && m_tgSPToggler.DestroyWindow();

        // Both togglers should be created to succeed
        return m_tgFPToggler.Create( *this ) && m_tgSPToggler.Create( *this );
    }

    // Once here this means its failure
    return FALSE;
}


/** 
 * 
 * Sets direction. Also resets mouse cursor based on direction
 * 
 * @param       eDirection_i - Direction to set.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SizingBar::SetDirection( const DIRECTION_e eDirection_i )
{
    if( eDirection_i != m_eDirection )
    {
        m_eDirection = eDirection_i;
        m_tgFPToggler.m_eDirection = m_tgSPToggler.m_eDirection = eDirection_i;
        
        // Get cursor id based on direction
        LPCTSTR lpctszCursor = ( eDirection_i == DIR_VERT ? IDC_SIZEWE : IDC_SIZENS );

        // Destroy old one
        DestroyCursor( m_hCursor );
        m_hCursor = LoadCursor( 0, m_bIsSizable ? lpctszCursor : IDC_ARROW );
        SetCursor( m_hCursor );
    }// End if
}// End SetDirection


/** 
 * 
 * Mouse movement event handler
 * 
 * @param       uFlags_i     - Mouse move flags
 * @param       cpMousePos_i - Current mouse position.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SizingBar::OnMouseMove( UINT uFlags_i, CPoint cpMousePos_i )
{
    // If not sizable quit
    if( !m_bIsSizable )
    {
        return;
    }
    
    // Check if we have LButton dragging
    if(( uFlags_i & MK_LBUTTON ) == MK_LBUTTON )
    {
        CRect crParentWindowRect;
        GetParent()->GetWindowRect( &crParentWindowRect );

        ClientToScreen( &cpMousePos_i );
        CPoint cpMoveDistance( cpMousePos_i.x - m_cpPreviousMousePosition.x, 
                               cpMousePos_i.y - m_cpPreviousMousePosition.y );

        if( !crParentWindowRect.PtInRect( cpMousePos_i ))
        {
            return;
        }

        if(( 0 == cpMoveDistance.x && DIR_VERT == m_eDirection ) || 
           ( 0 == cpMoveDistance.y && DIR_HORZ == m_eDirection ))
        {
            return;
        }

        // Our window is being dragged, tell dad about this ;)
        GetParent()->SendMessage( WM_REGISTERED_DRAGMSG, 
                                  RCAST( WPARAM, &cpMoveDistance ), 
                                  RCAST( LPARAM, this ));

        // Reset previous mouse position
        m_cpPreviousMousePosition = cpMousePos_i;
    }// End if
}// End OnMouseMove


/** 
 * 
 * For capturing mouse input when mouse button is down to simulate a drag.
 * 
 * @param       uFlags_i     - Click flags
 * @param       cpMousePos_i - Current mouse position.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SizingBar::OnLButtonDown( UINT uFlags_i, CPoint cpMousePos_i )
{
    UNREFERENCED_PARAMETER( uFlags_i );

    // Previous mouse position
    m_cpPreviousMousePosition = cpMousePos_i;
    ClientToScreen( &m_cpPreviousMousePosition );

    // Capture
    SetCapture();
    m_bIsSizing = true;
}


/** 
 * 
 * Releases captured mouse when mouse button is released.
 * 
 * @param       uFlags_i     - Mouse flags not used.
 * @param       cpMousePos_i - Current mouse position not used.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SizingBar::OnLButtonUp( UINT uFlags_i, CPoint cpMousePos_i )
{
    UNREFERENCED_PARAMETER( uFlags_i );
    UNREFERENCED_PARAMETER( cpMousePos_i );
    
    // Release captured mouse input, but before that check
    // that we are the one who has captured mouse input
    if( GetCapture() == this )
    {
        ReleaseCapture();
        m_bIsSizing = false;
    }
}


/** 
 * 
 * Overridden for setting mouse cursor based on orientation of sizing bar.
 * 
 * @param       pWnd_i     - Not used.
 * @param       nHitTest_i - Not used.
 * @param       uMessage_i - Not used.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL SizingBar::OnSetCursor( CWnd* pWnd_i, UINT nHitTest_i, UINT uMessage_i )
{
    UNREFERENCED_PARAMETER( pWnd_i );
    UNREFERENCED_PARAMETER( nHitTest_i );
    UNREFERENCED_PARAMETER( uMessage_i );
    
    // Set cursor
    if( m_hCursor )
    {
        ::SetCursor( m_hCursor );
        return TRUE;
    }
    else
    {
        return CWnd::OnSetCursor( pWnd_i, nHitTest_i, uMessage_i );
    }// End if
}


/** 
 * 
 * Message handler for WM_SIZE. Helps in resizing togglers based on this windows' size.
 * 
 * @param       uType_i - Type of sizing
 * @param       nCx_i   - New width
 * @param       nCy_i   - New height
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SizingBar::OnSize( UINT uType_i, int nCx_i, int nCy_i )
{
    // Return if either of the togglers is invalid
    if( !m_tgFPToggler.GetSafeHwnd() || !m_tgSPToggler.GetSafeHwnd() )
    {
        return;
    }

    // Delegate sizing message to first pane toggler
    m_tgFPToggler.SendMessage( WM_SIZE, uType_i, MAKELPARAM( nCx_i, nCy_i ));

    // Delegate sizing message to second pane toggler
    m_tgSPToggler.SendMessage( WM_SIZE, uType_i, MAKELPARAM( nCx_i, nCy_i ));

    // Call base class implementation
    CWnd::OnSize( uType_i, nCx_i, nCy_i );
}// End OnSize


/** 
 * 
 * Default constructor.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
DividerWnd::DividerWnd() : 
            m_pwndFirstPane( 0 ), 
            m_pwndSecondPane( 0 ),
            m_ermResizeMode( RM_EQUAL ),
            m_inDividerInsets( 0, 0, 0, 0 )
{}


/** 
 * 
 * Destructor does nothing.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
DividerWnd::~DividerWnd()
{}


ATOM DividerWnd::RegisterDividerWnd()
{
    // Window class
    WNDCLASSEX wcClassRegEx     = { 0 };
    wcClassRegEx.cbSize         = sizeof( wcClassRegEx );
    wcClassRegEx.hbrBackground  = ::GetSysColorBrush( COLOR_BTNFACE );
    wcClassRegEx.hCursor        = ::LoadCursor( 0, MAKEINTRESOURCE( IDC_ARROW ));
    wcClassRegEx.lpfnWndProc    = ::DefWindowProc;
    wcClassRegEx.lpszClassName  = DIVIDER_WND_CLASS;
    wcClassRegEx.style          = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    // Register class
    return ::RegisterClassEx( &wcClassRegEx );
}

BEGIN_MESSAGE_MAP(DividerWnd, CWnd)
    //{{AFX_MSG_MAP(DividerWnd)
	ON_WM_MOUSEMOVE()
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_REGISTERED_MESSAGE( WM_REGISTERED_DRAGMSG, DoSizingBarDragging )
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool DividerWnd::SetPane( CWnd& wndFirstPane_i,
                          const int nFirstPaneSpan_i,
                          CWnd& wndSecondPane_i,
                          const DIRECTION_e dirDividerDirection_i,
                          const int nSizingBarSpan_i )
{
    // If either of the window is invalid return
    if( !IsValidWindow( this )||                // This window should have been created
        !IsValidWindow( &GetSizingBar() ) ||    // SizingBar window should be valid
        !IsValidWindow( &wndFirstPane_i ) ||    // First pane should be valid
        !IsValidWindow( &wndSecondPane_i ))     // Second pane should be valid
    {
        ASSERT( FALSE );
        return false;
    }// End if

//    if( wndFirstPane_i.GetParent()->GetSafeHwnd() != GetSafeHwnd() ||   // Parent should be this window
//        wndSecondPane_i.GetParent()->GetSafeHwnd() != GetSafeHwnd() )   // Parent should be this window
//    {
//        ASSERT( FALSE );
//        return false;
//    }// End if

    // Assign first pane and second pane
    m_pwndFirstPane  = &wndFirstPane_i;
    m_pwndSecondPane = &wndSecondPane_i;

    // Direction of divider
    GetSizingBar().SetDirection( dirDividerDirection_i );

    // Initialize layout once
    InitLayout( nFirstPaneSpan_i, nSizingBarSpan_i );

    return true;
}// End SetPane


/** 
 * 
 * Resizes this window to fit parent window's client area. If parent
 * window size if not given then parent window size is calculated internally.
 * 
 * @param       pcrParentRect_i - Size of parent window.
 * @return      bool            - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DividerWnd::ResizeToFitParent( CRect* pcrParentRect_i )
{
    CRect crParentRect;

    // Validate parent rectangle
    if( pcrParentRect_i )
    {
        crParentRect = *pcrParentRect_i;
    }
    else
    {
        // Get parent client area since a valid rectangle is not passed in
        GetParent()->GetClientRect( &crParentRect );
    }// End if

    // Move this window into position
    MoveWindow( &crParentRect );

    // Successfully moved window
    return true;
}


/** 
 * 
 * Sets insets. Controls inside this window will be padded bases on these insets.
 * 
 * @param       inPadding_i - Amount of padding requested.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::SetInsets( const CInsets& inPadding_i )
{
    m_inDividerInsets = inPadding_i;

    if( IsValidWindow( this ))
    {
        // Get client rect
        CRect crClientRect;
        CWnd::GetClientRect( &crClientRect );

        // Refresh layout
        SendMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM( crClientRect.Width(), crClientRect.Height() ));
    }// End if
}// End SetInsets


/** 
 * 
 * Handles resize of window. So that panes are laid out correctly when
 * window is being resized.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::HandleResize()
{
    // If either of the window is invalid return
    if( !IsFirstPaneValid() || !IsSecondPaneValid() )
    {
        return;
    }

    // New client rectangle
    CRect crNewClientRect;
    GetClientRect( &crNewClientRect );

    // No need to resize if rectangle is empty
    if( crNewClientRect.IsRectEmpty() )
    {
         return;
    }
    
    // Get our resizing style
    switch( GetResizeMode() )
    {
        // Resize second pane, don't resize first pane
        case RM_SECONDPANE:
            {
                HandleSecondPaneResize( crNewClientRect );
                break;
            }// End case RM_SECONDPANE
        // Resize first pane, don't resize second pane
        case RM_FIRSTPANE:
            {
                HandleFirstPaneResize( crNewClientRect );
                break;
            }// End case RM_FIRSTPANE
        // Resize all panes equally.
        case RM_EQUAL:
            {
                HandleEqualResize( crNewClientRect );
                break;
            }// End case RM_EQUAL
    }// End switch

    Invalidate( FALSE );
}// End HandleResize


/** 
 * 
 * Manages equal resizing of panes.
 * 
 * @param       crNewClientRect_i - New client rectangle 
 * @return      bool              - Returns true if all went well
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DividerWnd::HandleEqualResize( const CRect& crNewClientRect_i )
{
    // Get first pane rectangle
    CRect crFirstPaneRect;
    m_pwndFirstPane->GetWindowRect( &crFirstPaneRect );
    ScreenToClient( &crFirstPaneRect );

    // Get divider rectangle
    CRect crSizingBarRect;
    GetSizingBar().GetWindowRect( &crSizingBarRect );
    ScreenToClient( &crSizingBarRect );

    // Get second pane rectangle
    CRect crSecondPaneRect;
    m_pwndSecondPane->GetWindowRect( &crSecondPaneRect );
    ScreenToClient( &crSecondPaneRect );

    if( GetSizingBar().IsVertical() )
    {
        // Get difference of new size and old size and divide it by two to 
        // get distance for each pane to be moved for eg: if distance is 4 then
        // nEqOffset will be 2 i.e. first pane will be moved by 2 and second pane
        // will also be resized by 2. If distance is 5 then first pane will 
        // be moved by 2 and second pane by 3.
        const int nEqOffset = ( crNewClientRect_i.right - crSecondPaneRect.right ) / 2;

        // Offset amount
        const CPoint cpOffset( nEqOffset, 0 );

        // Offset sizing bar rectangle first
        crSizingBarRect.OffsetRect( cpOffset );

        // Check if new sizing bar rectangle falls within our client rectangle or not
        if( !crNewClientRect_i.PtInRect( crSizingBarRect.TopLeft() ))
        {
            // Force sizing bar to fall within the client rectangle
            crSizingBarRect.OffsetRect( crNewClientRect_i.TopLeft() - crSizingBarRect.TopLeft() );
        }

        // Move sizing bar
        GetSizingBar().MoveWindow( &crSizingBarRect, FALSE );

        // Now resize first pane based on sizing bar's position
        crFirstPaneRect.right = crSizingBarRect.left;
        m_pwndFirstPane->MoveWindow( &crFirstPaneRect, FALSE );

        // Now resize second pane based on sizing bar's position
        crSecondPaneRect.left = crSizingBarRect.right;
        crSecondPaneRect.right = crNewClientRect_i.right;
        m_pwndSecondPane->MoveWindow( &crSecondPaneRect, FALSE );
    }
    else
    {
        // Get difference of new size and old size and divide it by two to 
        // get distance for each pane to be moved for eg: if distance is 4 then
        // nEqOffset will be 2 i.e. first pane will be moved by 2 and second pane
        // will also be resized by 2. If distance is 5 then first pane will 
        // be moved by 2 and second pane by 3.
        const int nEqOffset = ( crNewClientRect_i.bottom - crSecondPaneRect.bottom ) / 2;

        // Offset amount
        const CPoint cpOffset( 0, nEqOffset );

        // Offset sizing bar rectangle first
        crSizingBarRect.OffsetRect( cpOffset );

        // Check if new sizing bar rectangle falls within our client rectangle or not
        if( !crNewClientRect_i.PtInRect( crSizingBarRect.TopLeft() ))
        {
            // Force sizing bar to fall within the client rectangle
            crSizingBarRect.OffsetRect( crNewClientRect_i.TopLeft() - crSizingBarRect.TopLeft() );
        }

        // Now move sizing bar
        GetSizingBar().MoveWindow( &crSizingBarRect, FALSE );

        // Now resize first pane based on sizing bar's position
        crFirstPaneRect.bottom = crSizingBarRect.top;
        m_pwndFirstPane->MoveWindow( &crFirstPaneRect, FALSE );

        // Now resize second pane based on sizing bar's position
        crSecondPaneRect.top = crSizingBarRect.bottom;
        crSecondPaneRect.bottom = crNewClientRect_i.bottom;
        m_pwndSecondPane->MoveWindow( &crSecondPaneRect, FALSE );
    }// End if

    return true;

}// End HandleEqualResize

/** 
 * 
 * Handles resize of first pane.
 * 
 * @param       crNewClientRect_i - New client rectangle size
 * @return      bool              - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DividerWnd::HandleFirstPaneResize( const CRect& crNewClientRect_i )
{
    // Get first pane rectangle
    CRect crFirstPaneRect;
    m_pwndFirstPane->GetWindowRect( &crFirstPaneRect );
    ScreenToClient( &crFirstPaneRect );

    // Get divider rectangle
    CRect crSizingBarRect;
    GetSizingBar().GetWindowRect( &crSizingBarRect );
    ScreenToClient( &crSizingBarRect );

    // Get second pane rectangle
    CRect crSecondPaneRect;
    m_pwndSecondPane->GetWindowRect( &crSecondPaneRect );
    ScreenToClient( &crSecondPaneRect );

    if( GetSizingBar().IsVertical() )
    {
        // Get distance moved, since new 'right' will be stored in new 
        // client rectangle's 'right' and old 'right' will be stored in
        // second pane's 'right'.
        const int nXOffset = crNewClientRect_i.right - crSecondPaneRect.right;
        
        // Prepare offset point
        const CPoint cpOffset( nXOffset, 0 );
        
        // Offset divider first
        crSizingBarRect.OffsetRect( cpOffset );

        // Check if new sizing bar rectangle falls within our client rectangle or not
        if( !crNewClientRect_i.PtInRect( crSizingBarRect.TopLeft() ))
        {
            // Force sizing bar to fall within the client rectangle
            crSizingBarRect.OffsetRect( crNewClientRect_i.TopLeft() - crSizingBarRect.TopLeft() );
        }
        
        // Move sizing bar
        GetSizingBar().MoveWindow( &crSizingBarRect, FALSE );
        
        // Set first pane's right to divider's left
        crFirstPaneRect.right = crSizingBarRect.left;
        m_pwndFirstPane->MoveWindow( &crFirstPaneRect, FALSE );
        
        // Set second pane's left to divider rect's right, since it will
        // be to the right of divider.
        crSecondPaneRect.left = crSizingBarRect.right;
        
        // For safety sake set 'right' to the new client rectangle's right
        crSecondPaneRect.right = crNewClientRect_i.right;
        m_pwndSecondPane->MoveWindow( &crSecondPaneRect, FALSE );
    }
    else
    {
        // Get distance moved, since new 'bottom' will be stored in new client
        // rectangle's 'bottom', and old 'bottom' will be stored in 
        // second pane's 'bottom'
        const int nYOffset = crNewClientRect_i.bottom - crSecondPaneRect.bottom;
        
        // Prepare offset point
        const CPoint cpOffset( 0, nYOffset );
        
        // Offset divider first
        crSizingBarRect.OffsetRect( cpOffset );

        // Check if new sizing bar rectangle falls within our client rectangle or not
        if( !crNewClientRect_i.PtInRect( crSizingBarRect.TopLeft() ))
        {
            // Force sizing bar to fall within the client rectangle
            crSizingBarRect.OffsetRect( crNewClientRect_i.TopLeft() - crSizingBarRect.TopLeft() );
        }

        // Move sizing bar
        GetSizingBar().MoveWindow( &crSizingBarRect, FALSE );
        
        // Set first pane's bottom to divider's top
        crFirstPaneRect.bottom = crSizingBarRect.top;
        m_pwndFirstPane->MoveWindow( &crFirstPaneRect, FALSE );
        
        // Adjust second pane's size
        crSecondPaneRect.top = crSizingBarRect.bottom;
        
        // Make sure second pane's bottom always falls within client area of parent
        // window
        crSecondPaneRect.bottom = crNewClientRect_i.bottom;
        m_pwndSecondPane->MoveWindow( &crSecondPaneRect, FALSE );
    }// End if

    return true;
}// End HandleFirstPaneResize


/** 
 * 
 * Handles resize of second pane.
 * 
 * @param       crNewClientRect_i - New client rectangle
 * @return      bool              - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DividerWnd::HandleSecondPaneResize( const CRect& crNewClientRect_i )
{
    // Get first pane rectangle
    CRect crFirstPaneRect;
    m_pwndFirstPane->GetWindowRect( &crFirstPaneRect );
    ScreenToClient( &crFirstPaneRect );

    // Get divider rectangle
    CRect crSizingBarRect;
    GetSizingBar().GetWindowRect( &crSizingBarRect );
    ScreenToClient( &crSizingBarRect );

    // Get second pane rectangle
    CRect crSecondPaneRect;
    m_pwndSecondPane->GetWindowRect( &crSecondPaneRect );
    ScreenToClient( &crSecondPaneRect );

    DeferWindowPosMgr dwpDeferPos( false, true, this->GetSafeHwnd() );
    dwpDeferPos.Begin( 3 );
    if( GetSizingBar().IsVertical() )
    {
        // Check if new sizing bar rectangle falls within our client rectangle or not
        if( !crNewClientRect_i.PtInRect( crSizingBarRect.TopLeft() ))
        {
            CPoint cpOffset( crNewClientRect_i.right - ( crSizingBarRect.right ), 0 );

            // Force sizing bar to fall within the client rectangle
            crSizingBarRect.OffsetRect( cpOffset );

            // Reset first pane rectangles left, since it might also
            // have gone out of bounds
            crFirstPaneRect.right = crSizingBarRect.left;

            // Add first pane window to move back into position
            dwpDeferPos.AddMoveWindow( *m_pwndFirstPane, crFirstPaneRect );
        }// End if

        // Add a window for moving
        dwpDeferPos.AddMoveWindow( GetSizingBar(), crSizingBarRect );

        // Set left and right of second pane rectangle
        crSecondPaneRect.left = crSizingBarRect.right;

        // Set right of second pane rectangle to new client rectangle's right
        crSecondPaneRect.right = crNewClientRect_i.right;

        // Move window into position.
        dwpDeferPos.AddMoveWindow( *m_pwndSecondPane, crSecondPaneRect );
    }
    else
    {
        // Check if new sizing bar rectangle falls within our client rectangle or not
        if( !crNewClientRect_i.PtInRect( crSizingBarRect.TopLeft() ))
        {
            CPoint cpOffset( 0, crNewClientRect_i.bottom - ( crSizingBarRect.bottom ));

            // Force sizing bar to fall within the client rectangle
            crSizingBarRect.OffsetRect( cpOffset );

            // Reset first pane rectangles left, since it might also
            // have gone out of bounds
            crFirstPaneRect.bottom = crSizingBarRect.top;

            // Add first pane window to move back into position
            dwpDeferPos.AddMoveWindow( *m_pwndFirstPane, crFirstPaneRect );
        }// End if

        // Add a window for moving
        dwpDeferPos.AddMoveWindow( GetSizingBar(), crSizingBarRect );

        // Set left and right of second pane rectangle
        crSecondPaneRect.top = crSizingBarRect.bottom;

        // Set right of second pane rectangle to new client rectangle's right
        crSecondPaneRect.bottom = crNewClientRect_i.bottom;

        // Move window into position.
        dwpDeferPos.AddMoveWindow( *m_pwndSecondPane, crSecondPaneRect );
    }// End if

    return true;

}// End HandleSecondPaneResize


/** 
 * 
 * Handles sizing of Divider main window. Sizes all child controls so that they
 * fit into main divider window. 
 * 
 * @param       uType_i - Type of sizing done. Not used here.
 * @param       nCx_i   - New width.
 * @param       nCy_i   - New height.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::OnSize( UINT uType_i, int nCx_i, int nCy_i )
{
    UNREFERENCED_PARAMETER( uType_i );
    UNREFERENCED_PARAMETER( nCx_i );
    UNREFERENCED_PARAMETER( nCy_i );

    // Get direction of divider
    const DIRECTION_e& dirDividerDirection = GetSizingBar().GetDirection();

    // Get new client rectangle
    CRect crNewClientRect;
    GetClientRect( crNewClientRect );

    // Resize all child window's
    CWnd* pChild = GetWindow( GW_CHILD );

    // Manages resizing of windows
    DeferWindowPosMgr dwpmDeferPosMgr( false, true, this->GetSafeHwnd() );
    VERIFY( dwpmDeferPosMgr.Begin( 3 ));

    while( pChild )
    {
        // Get current co-ordinates of child window
        CRect crChildWindowRectangle;
        pChild->GetWindowRect( crChildWindowRectangle );
        ScreenToClient( &crChildWindowRectangle );

        CSize csNewSize;

        if( dirDividerDirection == DIR_VERT )
        {
            csNewSize.cx = crChildWindowRectangle.Width();
            csNewSize.cy = crNewClientRect.Height();
        }
        else
        {
            csNewSize.cx = crNewClientRect.Width();
            csNewSize.cy = crChildWindowRectangle.Height();
        }

        // Put one window to resize
        dwpmDeferPosMgr.AddSizeWindow( *pChild, csNewSize );

        // Get next child window of this window
        pChild = pChild->GetWindow( GW_HWNDNEXT );
    }// End while

    // Resize all put windows at one go
    dwpmDeferPosMgr.End();

    // Do special handling of pane size
    HandleResize();
}// End OnSize


afx_msg BOOL DividerWnd::OnEraseBkgnd( CDC* pDC_i )
{
    UNREFERENCED_PARAMETER( pDC_i );
    return GetSizingBar().IsSizing();
}


/** 
 * 
 * Returns a constant reference to a pane from given pane id.
 * 
 * @param       ePane_i     - Pane id.
 * @return      const CWnd* -  constance reference to pane
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
const CWnd* DividerWnd::GetPane( const PANE_e ePane_i ) const
{
    if( ePane_i == PANE_FIRST )
    {
        return m_pwndFirstPane;
    }
    else if( ePane_i == PANE_SECOND )
    {
        return m_pwndSecondPane;
    }
    else
    {
        ASSERT( false );
        return 0;
    }// End if
}// End GetPane


/** 
 * 
 * Sets span of divider.
 * 
 * @param       nSizingBarSpan_i - Space occupied by sizing bar
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::SetSizingBarSpan( const int nSizingBarSpan_i )
{
    if( IsValidWindow( &GetSizingBar() ))
    {
        CRect crSizingBarRect;
        GetSizingBar().GetWindowRect( crSizingBarRect );
        if( DIR_VERT == GetSizingBar().GetDirection() )
        {
            crSizingBarRect.right = crSizingBarRect.left + nSizingBarSpan_i;
        }
        else
        {
            crSizingBarRect.bottom = crSizingBarRect.top + nSizingBarSpan_i;
        }// End if
    }// End if
}// End SetDividerSpan


/** 
 * 
 * Changes layout of sizing bar.
 * 
 * @param       ePaneDirection_i - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::SetSizingBarDirection( const DIRECTION_e ePaneDirection_i )
{
    // Check whether direction changed or not
    if( ePaneDirection_i == GetSizingBar().GetDirection() )
    {
        return;
    }

    // Get client rectangle
    CRect crClientRect;
    GetClientRect( &crClientRect );

    // Get rectangle of first pane
    CRect crFirstPaneRect;
    m_pwndFirstPane->GetWindowRect( &crFirstPaneRect );
    ScreenToClient( &crFirstPaneRect );

    int nFirstPaneSpan = 0;
    if( GetSizingBar().IsVertical() )
    {
        // Get percentage of span when compared to client rectangle
        nFirstPaneSpan = SCAST( int, 
                                ( 
                                    SCAST( float, crFirstPaneRect.Width() ) / 
                                    SCAST( float, crClientRect.Width() )
                                ) * 100.0f + 0.5f
                              );

        nFirstPaneSpan = SCAST( int,
                                (
                                    SCAST( float, nFirstPaneSpan ) /
                                    100.0f
                                ) * crClientRect.Height() + 0.5f
                              );
    }
    else
    {
        // Get percentage of span when compared to client rectangle
        nFirstPaneSpan = SCAST( int, 
                                ( 
                                  SCAST( float, crFirstPaneRect.Height() ) / 
                                  SCAST( float, crClientRect.Height() )
                                ) * 100.0f + 0.5f
                              );

        nFirstPaneSpan = SCAST( int,
                                (
                                    SCAST( float, nFirstPaneSpan ) /
                                    100.0f
                                ) * crClientRect.Width() + 0.5f
                              );
    }// End if

    CRect crSizingBarRect;
    GetSizingBar().GetWindowRect( &crSizingBarRect );
    ScreenToClient( &crSizingBarRect );

    // Get previous direction
    const DIRECTION_e ePreviousDirection = GetSizingBar().GetDirection();

    GetSizingBar().SetDirection( ePaneDirection_i );
    InitLayout( nFirstPaneSpan, 
                ( DIR_VERT == ePreviousDirection ? crSizingBarRect.Width() : crSizingBarRect.Height() ));
}// End SetSizingBarDirection


/** 
 * 
 * Called once before control are being laid out so that the controls are positioned 
 * correctly.
 * 
 * @param       nFirstPaneSpan_i - Span/Space of first pane.
 * @param       nSizingBarSpan_i - Span/Space of sizing bar.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::InitLayout( const int nFirstPaneSpan_i, const int nSizingBarSpan_i )
{
    // Get main client rectangle
    CRect crMainClientRect;
    GetClientRect( &crMainClientRect );

    DeferWindowPosMgr dwpmDeferPosMgr( false, true, this->GetSafeHwnd() );
    VERIFY( dwpmDeferPosMgr.Begin( 3 ));    

    if( GetSizingBar().IsVertical() )
    {
        // Set first pane rectangle
        CRect crFirstPaneRect = crMainClientRect;
        crFirstPaneRect.right = crFirstPaneRect.left + nFirstPaneSpan_i; //Set right of first pane
        m_pwndFirstPane->MoveWindow( &crFirstPaneRect, FALSE );
        dwpmDeferPosMgr.AddMoveWindow( *m_pwndFirstPane, crFirstPaneRect );

        // Set divider window rectangle
        CRect crSizingBarRect = crMainClientRect;
        crSizingBarRect.left = crFirstPaneRect.right; // Set left of divider window
        crSizingBarRect.right = crSizingBarRect.left + nSizingBarSpan_i; // Set span of divider
        dwpmDeferPosMgr.AddMoveWindow( GetSizingBar(), crSizingBarRect );

        // Set second pane rectangle
        CRect crSecondPaneRectangle = crMainClientRect;
        crSecondPaneRectangle.left = crSizingBarRect.right;
        dwpmDeferPosMgr.AddMoveWindow( *m_pwndSecondPane, crSecondPaneRectangle );
    }
    else
    {
        // Set first pane rectangle
        CRect crFirstPaneRect = crMainClientRect;
        crFirstPaneRect.bottom = crMainClientRect.top + nFirstPaneSpan_i;//Set bottom of first pane
        dwpmDeferPosMgr.AddMoveWindow( *m_pwndFirstPane, crFirstPaneRect );

        // Set divider window rectangle
        CRect crSizingBarRect = crMainClientRect;
        crSizingBarRect.top = crFirstPaneRect.bottom; // Set left of divider window
        crSizingBarRect.bottom = crSizingBarRect.top + nSizingBarSpan_i; // Set span of divider
        dwpmDeferPosMgr.AddMoveWindow( GetSizingBar(), crSizingBarRect );

        // Set second pane rectangle
        CRect crSecondPaneRectangle = crMainClientRect;
        crSecondPaneRectangle.top = crSizingBarRect.bottom;
        dwpmDeferPosMgr.AddMoveWindow( *m_pwndSecondPane, crSecondPaneRectangle );
    }// End if

    // Move all windows into position
    dwpmDeferPosMgr.End();

    Invalidate();
}// End InitLayout


/** 
 * 
 * This message is sent by sizing bar when it's being dragged. First and
 * second pane resizes themselves based on this message and position of 
 * sizing bar.
 * 
 * @param       wMousePosition_i - A CPoint indicating the distance dragged.
 * @param       lUnUsed_i        - This parameter is not used.
 * @return      LRESULT          - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
LRESULT DividerWnd::DoSizingBarDragging( WPARAM wMousePosition_i, LPARAM lUnUsed_i )
{
    UNREFERENCED_PARAMETER( lUnUsed_i );

    DeferWindowPosMgr dwpmDeferPosMgr( false, true, this->GetSafeHwnd() );
    VERIFY( dwpmDeferPosMgr.Begin( 3 ) );

    // Check given parameters
    if( !wMousePosition_i )
    {
        return 0;
    }

    // Mouse move distance
    CPoint& cpMousePosition = *RCAST( CPoint*, wMousePosition_i );

    // For forcing all children to lie within parent's client area
    const CRect crParentWindowRect;

    // For a moment remove constantness and get the client rect but for the rest of
    // the function the above variable will be constant
    GetClientRect( CCAST( LPRECT, SCAST( LPCRECT, &crParentWindowRect )));

    // First Move sizing bar window into position
    CRect crSizingBarRect;
    GetSizingBar().GetWindowRect( &crSizingBarRect );
    ScreenToClient( &crSizingBarRect );
    if( GetSizingBar().IsVertical() )
    {
        crSizingBarRect.left += cpMousePosition.x;
        crSizingBarRect.right += cpMousePosition.x;
    }
    else
    {
        crSizingBarRect.top += cpMousePosition.y;
        crSizingBarRect.bottom += cpMousePosition.y;
    }

    // Reposition sizing bar
    VERIFY( dwpmDeferPosMgr.AddRepositionWindow( GetSizingBar(), crSizingBarRect.TopLeft() ));

    // Based on divider window's position reposition first and second panes
    CRect crFirstPaneRect;
    m_pwndFirstPane->GetWindowRect( &crFirstPaneRect );
    ScreenToClient( &crFirstPaneRect );
    if( GetSizingBar().IsVertical() )
    {
        crFirstPaneRect.right = crSizingBarRect.left;
    }
    else
    {
        crFirstPaneRect.bottom = crSizingBarRect.top;
    }

    // Move first pane
    VERIFY( dwpmDeferPosMgr.AddMoveWindow( *m_pwndFirstPane, crFirstPaneRect ));

    // Now handle second  pane rectangle
    CRect crSecondPaneRect;
    m_pwndSecondPane->GetWindowRect( &crSecondPaneRect );
    ScreenToClient( &crSecondPaneRect );
    if( GetSizingBar().IsVertical() )
    {
        crSecondPaneRect.left = crSizingBarRect.right;

        // Make sure right is withing parent's right
        crSecondPaneRect.right = crParentWindowRect.right;
    }
    else
    {
        crSecondPaneRect.top = crSizingBarRect.bottom;

        // Make sure bottom is within parent's bottom
        crSecondPaneRect.bottom = crParentWindowRect.bottom;
    }

    // Add move second pane
    VERIFY( dwpmDeferPosMgr.AddMoveWindow( *m_pwndSecondPane, crSecondPaneRect ));

    // Move all windows into position
    dwpmDeferPosMgr.End();

    // Request a repaint
    Invalidate( FALSE );

    // Return success
    return 1;
}// End DoDividerDragging


/** 
 * 
 * Sets focus to first pane when this control is focussed.
 * 
 * @param       Nil
 * @return      CWnd* - Returns focussed window
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
CWnd* DividerWnd::SetFocus()
{
    if( IsFirstPaneValid() )
    {
        return m_pwndFirstPane->SetFocus();
    }

    return CWnd::SetFocus();
}


/** 
 * 
 * Divider window's own create function. The other two create functions of CWnd is disabled. 
 * Users of this class has to call this function to create divider window.
 * 
 * @param       crWindowRect_i     - Size of divider window.
 * @param       pParent_i          - Parent of divider window
 * @param       uID_i              - Id of divider window
 * @param       dwStyleWnd_i       - Style of divider window
 * @param       dwStyleWndEx_i     - Extended styles if any.
 * @param       dwStyleDivider_i   - Style of embedded divider
 * @param       dwStyleDividerEx_i - Extended style of embedded divider.
 * @return      BOOL               - Returns TRUE if successfully created else FALSE.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
BOOL DividerWnd::Create( const CRect& crWindowRect_i, 
                         CWnd* pParent_i,
                         const UINT uID_i,
                         const DWORD dwStyleWnd_i,
                         const DWORD dwStyleWndEx_i,
                         const DWORD dwStyleSizingBar_i,
                         const DWORD dwStyleSizingBarEx_i )
{
    // Create this window first
    if( IsValidWindow( this ) || // If already created or
        CWnd::CreateEx( dwStyleWndEx_i, 
                        DIVIDER_WND_CLASS, 
                        0, 
                        dwStyleWnd_i, 
                        crWindowRect_i, 
                        pParent_i, 
                        uID_i )
                        ) // Created
    {
        // If already created then destroy
        GetSizingBar().GetSafeHwnd() && GetSizingBar().DestroyWindow();

        // Now create SizingBar window
        return GetSizingBar().Create( *this, dwStyleSizingBar_i, dwStyleSizingBarEx_i );
    }// End if

    // Once here this means creation failed
    return FALSE;
}// End Create


/** 
 * 
 * Returns client rectangle.
 * 
 * @param       lpRect_o - On return will hold client rectangle
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DividerWnd::GetClientRect( LPRECT lpRect_o )
{
    CWnd::GetClientRect( lpRect_o );

    lpRect_o->left += m_inDividerInsets.left;
    lpRect_o->top += m_inDividerInsets.top;
    lpRect_o->right -= m_inDividerInsets.right;
    lpRect_o->bottom -= m_inDividerInsets.bottom;
}