/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DeferWindowPosMgr.cpp - Implementation file for DeferWindowPosMgr class
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */

#include "stdafx.h"
#include "DeferWindowPosMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/** 
 * 
 * A utility constructor with all parameters having default values, hence also
 * acts as the default constructor.
 * 
 * @param       bRepaintOnDeferPos_i - Repaint on defer pos flag
 * @param       bEnableSetRedraw_i   - Set redraw flag
 * @param       hWindow_i            - Window to redraw
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
DeferWindowPosMgr::DeferWindowPosMgr( const bool bRepaintOnDeferPos_i, 
                                      const bool bEnableSetRedraw_i, 
                                      HWND hWindow_i ) : m_hdwp( 0 ),
                                                         m_bRepaintOnDeferPos( bRepaintOnDeferPos_i ),
                                                         m_bSetRedrawEnabled( bEnableSetRedraw_i ),
                                                         m_hRedrawWindow( hWindow_i )
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
DeferWindowPosMgr::~DeferWindowPosMgr()
{
    End();
}


/** 
 * 
 * Attaches a defer pos structure. Structure won't be attached if
 * we already have another valid hdwp on our hand.
 * 
 * @param       hDeferPosStruct_i - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DeferWindowPosMgr::AttachHDWP( HDWP& hDeferPosStruct_i )
{
    if( m_hdwp || !hDeferPosStruct_i )
    {
        ASSERT( FALSE );
        return false;
    }

    // Attach
    m_hdwp = hDeferPosStruct_i;

    // Successfully attached
    return true;
}


/** 
 * 
 * Sets repaint status to given status. This flag while moving, sizing, or repositioning windows.
 * Determines whether repaint is required while these actions are performed.
 * 
 * @param       bRepaintOnDeferPos_i - Repaint status of windows.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DeferWindowPosMgr::SetRepaintOnDeferPos( const bool bRepaintOnDeferPos_i )
{
    m_bRepaintOnDeferPos = bRepaintOnDeferPos_i;
}


/** 
 * 
 * Returns repaint status.
 * 
 * @param       Nil
 * @return      bool - Return status
 * @exception   Nil
 * @see         SetRepaintOnDeferPos
 * @since       1.0
 */
bool DeferWindowPosMgr::IsRepaintOnDeferPosEnabled()
{
    return m_bRepaintOnDeferPos;
}


/** 
 * 
 * Specifies whether SetRedraw function should be used in Conjunction with
 * BeginDeferWindowPos and EndDeferWindowPos function.
 *
 * Calls SetRedraw( FALSE ) on Begin and SetRedraw( TRUE ) on end.
 * 
 * @param       bRedrawEnabled_i - Sets flag
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void DeferWindowPosMgr::SetRedrawEnabled( const bool bRedrawEnabled_i, HWND& hWindow_i )
{
    m_bSetRedrawEnabled = bRedrawEnabled_i;
    m_hRedrawWindow = hWindow_i;
}


/** 
 * 
 * Returns value of redraw enabled flag.
 * 
 * @param       Nil
 * @return      bool - Return value of flag
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DeferWindowPosMgr::IsRedrawEnabled()
{
    return m_bSetRedrawEnabled;
}


/** 
 * 
 * Begins layout of windows by calling BeginDeferWindowPos
 * 
 * @param       nWindowCount_i - Number of windows to be resized.
 * @return      void
 * @exception   Nil
 * @see         End
 * @since       1.0
 */
bool DeferWindowPosMgr::Begin( const int nWindowCount_i )
{
    m_hdwp = BeginDeferWindowPos( nWindowCount_i );

    // If redraw is enabled then disable drawing.
    IsRedrawEnabled() && m_hRedrawWindow && ::SendMessage( m_hRedrawWindow, WM_SETREDRAW, FALSE, 0 );

    // Return status;
    return m_hdwp != 0;
}


/** 
 * 
 * Calls EndDeferWindowPos.
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Begin
 * @since       1.0
 */
bool DeferWindowPosMgr::End()
{
    BOOL bResult = FALSE;
    if( m_hdwp )
    {
        bResult = EndDeferWindowPos( m_hdwp );
        m_hdwp = 0;
    }

    // If redraw is enabled then enable drawing.
    IsRedrawEnabled() && m_hRedrawWindow && ::SendMessage( m_hRedrawWindow, WM_SETREDRAW, TRUE, 0 );

    // Return status
    return bResult != FALSE;
}// End


/** 
 * 
 * Adds a window to resize but no move.
 * 
 * @param       wndToSize_i - Window to resize
 * @param       sizNewSize_i - New size
 * @return      bool        - Return true if successfully resized.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DeferWindowPosMgr::AddSizeWindow( CWnd& wndToSize_i, 
                                       const SIZE& sizNewSize_i,
                                       HWND pwndInsertAfter_i,
                                       UINT uPosFlags_i )
{
    // Switch off repaint if repaint flag is off
    !m_bRepaintOnDeferPos && (( uPosFlags_i |= SWP_NOREDRAW ) &= ~SWP_DRAWFRAME );

    // Defer pos only if defer pos structure is valid
    if( m_hdwp )
    {
        m_hdwp = DeferWindowPos( m_hdwp, 
                                 wndToSize_i.GetSafeHwnd(), 
                                 pwndInsertAfter_i, 
                                 0, 
                                 0,     
                                 sizNewSize_i.cx, 
                                 sizNewSize_i.cy, 
                                 uPosFlags_i );
    }// End if

    // Well if call failed use our very own SetWindowPos as a backup action ;)
    // We don't want window movement to get stuck because of this failure
    if( 0 == m_hdwp )
    {
        return SetWindowPos( wndToSize_i.GetSafeHwnd(),
                             pwndInsertAfter_i, 
                             0, 
                             0,     
                             sizNewSize_i.cx, 
                             sizNewSize_i.cy, 
                             uPosFlags_i ) != FALSE;
    }// End if

    // Return status
    return true;
}// End AddSizeWindow


/** 
 * 
 * Adds a window to move but no resize
 * 
 * @param       wndToReposition_i - Window to reposition
 * @param       ptNewPosition_i   - New position of window
 * @return      bool              - Return true on success
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DeferWindowPosMgr::AddRepositionWindow( CWnd& wndToReposition_i, 
                                             const POINT& ptNewPosition_i,
                                             HWND pwndInsertAfter_i,
                                             UINT uPosFlags_i )
{
    // Switch off repaint if repaint flag is off
    !m_bRepaintOnDeferPos && (( uPosFlags_i |= SWP_NOREDRAW ) &= ~SWP_DRAWFRAME );

    // Defer pos only if defer struct is valid
    if( m_hdwp )
    {
        m_hdwp = DeferWindowPos( m_hdwp, 
                                 wndToReposition_i.GetSafeHwnd(), 
                                 pwndInsertAfter_i, 
                                 ptNewPosition_i.x, 
                                 ptNewPosition_i.y, 
                                 0, 
                                 0, 
                                 uPosFlags_i );
    }// End if

    // Well if call failed use our very own SetWindowPos as a backup action ;)
    // We don't want window movement to get stuck because of this failure
    if( 0 == m_hdwp )
    {
        return SetWindowPos( wndToReposition_i.GetSafeHwnd(),
                             pwndInsertAfter_i,
                             ptNewPosition_i.x, 
                             ptNewPosition_i.y,     
                             0, 
                             0, 
                             uPosFlags_i ) != FALSE;
    }// End if

    return true;
}// End AddRepositionWindow


/** 
 * 
 * Adds a window to move and resize.
 * 
 * @param       wndToMove_i   - Window to move.
 * @param       rcNewCoords_i - New co-ordinates of window.
 * @return      bool          - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool DeferWindowPosMgr::AddMoveWindow( CWnd& wndToMove_i, 
                                       const RECT& rcNewCoords_i, 
                                       HWND pwndInsertAfter_i,
                                       UINT uPosFlags_i )
{
    // Switch off repaint if repaint flag is off
    !m_bRepaintOnDeferPos && (( uPosFlags_i |= SWP_NOREDRAW ) &= ~SWP_DRAWFRAME );

    // Defer pos only if defer pos structure is valid
    if( m_hdwp )
    {
        m_hdwp = DeferWindowPos( m_hdwp, 
                                 wndToMove_i.GetSafeHwnd(), 
                                 pwndInsertAfter_i, 
                                 rcNewCoords_i.left, 
                                 rcNewCoords_i.top, 
                                 rcNewCoords_i.right - rcNewCoords_i.left, 
                                 rcNewCoords_i.bottom - rcNewCoords_i.top, 
                                 uPosFlags_i );
    }// End if

    // Well if call failed use our very own SetWindowPos as a backup action ;)
    // We don't want window movement to get stuck because of this failure
    if( 0 == m_hdwp )
    {
        return SetWindowPos( wndToMove_i.GetSafeHwnd(),
                             pwndInsertAfter_i, 
                             rcNewCoords_i.left, 
                             rcNewCoords_i.top, 
                             rcNewCoords_i.right - rcNewCoords_i.left, 
                             rcNewCoords_i.bottom - rcNewCoords_i.top, 
                             uPosFlags_i ) != FALSE;
    }// End if

    // Return success
    return true;
}// End AddMoveWindow