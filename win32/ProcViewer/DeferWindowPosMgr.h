/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DeferWindowPosMgr.h - Contains interface declaration for DeferWindowPosMgr class.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */


#ifndef _DEFER_WINDOW_POS_MGR_H_
#define _DEFER_WINDOW_POS_MGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DeferWindowPosMgr - Class that manages defer window position API functions.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */
class DeferWindowPosMgr  
{
    public:

        DeferWindowPosMgr( const bool bRepaintOnDeferPos_i = false, const bool bEnableSetRedraw_i = false, HWND hWindow_i = 0 );

        // Destructor
	    virtual ~DeferWindowPosMgr();

        // Attaches a defer pos structure. Structure won't be attached if
        // we already have another valid hdwp on our hand
        bool AttachHDWP( HDWP& hDeferPosStruct_i );

        // Set repaint status flag
        void SetRepaintOnDeferPos( const bool bRepaintOnDeferPos_i );
        bool IsRepaintOnDeferPosEnabled();

        // Enables/Disables SetRedraw 
        void SetRedrawEnabled( const bool bRedrawEnabled_i, HWND& hWindow_i );
        bool IsRedrawEnabled();

        // Helper for BeginDeferWindowPos
        bool Begin( const int nWindowCount_i );
        bool End();

        // Add a window to only size, no move
        bool AddSizeWindow( CWnd& wndToSize_i, 
                            const SIZE& sizNewSize_i,
                            HWND pwndInsertAfter_i = 0,
                            UINT uPosFlags_i = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );

        // Add a window to only move, no size
        bool AddRepositionWindow( CWnd& wndToReposition_i, 
                                  const POINT& ptNewPosition_i, 
                                  HWND pwndInsertAfter_i = 0,
                                  UINT uPosFlags_i = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );

        // Add a window to both move and size
        bool AddMoveWindow( CWnd& wndToMove_i, 
                            const RECT& rcNewCoords_i, 
                            HWND pwndInsertAfter_i = 0,
                            UINT uPosFlags_i = SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );

    private:

        HDWP m_hdwp;

        bool m_bRepaintOnDeferPos;

        bool m_bSetRedrawEnabled;
        HWND m_hRedrawWindow;

};// End DeferWindowPosMgr

#endif // _DEFER_WINDOW_POS_MGR_H_
