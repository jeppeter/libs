/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DividerWnd.h - Interface declaration for DividerWnd
 *
 * Classes declared in this file: DividerWnd, SizingBar, Toggler
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-01
 */


#ifndef _DIVIDER_WND_H_
#define _DIVIDER_WND_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
    #define DBREAK( condition ) if( condition ) _asm int 3
#else
    #define DBREAK( condition )
#endif 

#define TOGGLER_CLASS _T( "ClsToggler -- Jesus christ is lord and savior" )
#define SIZING_BAR_CLASS _T( "ClsNibuSizingBar -- Jesus christ is lord and savior" )
#define DIVIDER_WND_CLASS _T( "ClsDividerMainWnd -- Jesus christ is lord and savior" )

#ifdef RCAST
    #undef RCAST
#endif
#define RCAST( type, dt ) ( reinterpret_cast<type>(( dt )))

#ifdef SCAST
    #undef SCAST
#endif
#define SCAST( type, dt ) ( static_cast<type>(( dt )))

#ifdef CCAST
    #undef CCAST
#endif
#define CCAST( type, dt ) ( const_cast<type>(( dt )))

// Extern variables used for getting our classes registered exactly once.
extern const ATOM g_nRegisterSizingBarInvoker, 
                  g_nRegisterDividerWndInvoker, 
                  g_nRegisterToggler;

// Default width of divider
#define DEFAULT_SIZING_BAR_SPAN 8

// Constant that identifies a registered drag message
extern const UINT WM_REGISTERED_DRAGMSG;


/** 
 * 
 * Returns validity status of a given window. Check whether given window pointer
 * is not null and it's internal handle is valid or not.
 * 
 * @param       pwndToCheck_i - Window to check for validity
 * @return      bool          - Returns true if window is valid
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
__inline bool IsValidWindow( const CWnd* pwndToCheck_i )
{
	return ( pwndToCheck_i && ::IsWindow( pwndToCheck_i->GetSafeHwnd() ) == TRUE );
}

// Easiest way to create a new class ;)
typedef CRect CInsets;


/** 
 * 
 * Just resizes a given window. Does not moves the window. We inhibit moving 
 * by using SWP_NOMOVE flag.
 * 
 * @param       wndWindowToResize_i - Window to be resized.
 * @param       csNewSize_i         - New size of window
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
__inline bool ResizeWnd( CWnd& wndWindowToResize_i, const CSize& csNewSize_i )
{
    return wndWindowToResize_i.SetWindowPos( 0, 
                                             0, 
                                             0, 
                                             csNewSize_i.cx, 
                                             csNewSize_i.cy,
                                             SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME ) != FALSE;
}


/** 
 * 
 * Just repositions given windows does not resize it. We inhibit movement by using SWP_NOSIZE.
 * 
 * @param       wndWindowToMove_i - Window to be repositioned.
 * @param       cpNewPosition     - New position of window.
 * @return      bool              - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
__inline bool RepositionWnd( CWnd& wndWindowToMove_i, const CPoint& cpNewPosition )
{
    return wndWindowToMove_i.SetWindowPos( 0, 
                                           cpNewPosition.x, 
                                           cpNewPosition.y, 
                                           0, 
                                           0, 
                                           SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME ) != FALSE;
}


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * PANE_e - Constants to identify panes.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */
enum PANE_e
{
	PANE_FIRST = 100,
	PANE_SECOND
};


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DIRECTION_e - Constants to identify sizing bar orientations.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */
enum DIRECTION_e
{
    DIR_HORZ = 1201,
    DIR_VERT = 10201
};


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * RESIZE_MODE_e - Constants to identify resize mode
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */
enum RESIZE_MODE_e
{
    RM_SECONDPANE = 123,    // Only resizes second pane, first pane remains fixed
    RM_FIRSTPANE,           // Only resized first pane, second pane remains fixed
    RM_EQUAL                // Equally resizes both panes
};


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * Toggler - Toggler to be drawn on sizing bar. Purpose is to toggle the size of associated pane.
 *           Which pane to be associated with is to be decided by the sizing bar. One to one mapping 
 *           between toggler and it's pane. 
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-08
 */
class Toggler : public CWnd
{
    public:

        static ATOM RegisterToggler();
    
    private:

        // Disable CWnd Create and CreateEx
        using CWnd::Create;
        using CWnd::CreateEx;

        // Handles onsize
        void OnSize( UINT uType_i, int nCx_i, int nCy_i );

        friend class SizingBar;

        Toggler( const DIRECTION_e m_eDirection );
        ~Toggler();

        // Creates toggler based on parent windows size
        BOOL Create( CWnd& wndParent_i );

        // Set direction of toggler
        void SetDirection( const DIRECTION_e eDirection_i );

        // Width of component before it was toggled
        int m_nPaneSpanBeforeToggling;

        // Span of toggler
        int m_nTogglerSpan;

        // Direction of sizing bar, this direction will decide the orientation of toggler
        // i.e whether it should be 
        // ->, <-  or  |   , / \
        //            \ /     |
        DIRECTION_e m_eDirection;

        DECLARE_MESSAGE_MAP();

};// End Toggler


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * SizingBar - Sizing bar used within divider window.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-07
 */
class SizingBar : public CWnd
{
    friend class DividerWnd;

    public:
    
        // Function for registering sizing bar
        static ATOM RegisterSizingBar();

        // Returns direction
        DIRECTION_e GetDirection() const
        {
            return m_eDirection;
        }

        // Returns true if sizing bar direction is vertical
        bool IsVertical() const
        {
            return DIR_VERT == m_eDirection;
        }

        // Returns true if sizing bar direction is horizontal
        bool IsHorizontal() const
        {
            return DIR_HORZ == m_eDirection;
        }

        bool IsSizing() const
        {
            return m_bIsSizing;
        }

    private:

        SizingBar( const DIRECTION_e eDirection_i = DIR_VERT );
        ~SizingBar();

        BOOL Create( CWnd& wndParent_i, const DWORD dwStyle_i, const DWORD dwStyleEx_i );
        void SetDirection( const DIRECTION_e eDirection_i );

        void OnMouseMove( UINT uFlags_i, CPoint cpMousePos_i );
        void OnLButtonDown( UINT uFlags_i, CPoint cpMousePos_i );
        void OnLButtonUp( UINT uFlags_i, CPoint cpMousePos_i );
        BOOL OnSetCursor( CWnd* pWnd_i, UINT nHitTest_i, UINT uMessage_i );

    private:

        // Message handler for WM_SIZE
        void OnSize( UINT uType_i, int nCx_i, int nCy_i );

        // Disable external calls to create functions, 
        // only DividerWnd can create this class
        using CWnd::Create;
        using CWnd::CreateEx;

        CPoint m_cpPreviousMousePosition;
        DIRECTION_e m_eDirection;
        HCURSOR m_hCursor;

        // First pane toggler
        Toggler m_tgFPToggler;

        // Second pane toggler
        Toggler m_tgSPToggler;

        bool m_bIsSizable;
        bool m_bIsSizing;

        DECLARE_MESSAGE_MAP()

};// End SizingBar


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * DividerWnd - Main divider window. Manages auto relayout of child windows based
 * on sizing bar drag/window resize.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-04-07
 */
class DividerWnd : public CWnd
{
    friend class SizingBar;

    // Construction
    public:

	    DividerWnd();
        ~DividerWnd();

        bool SetPane( CWnd& wndFirstPane_i,
                      const int nFirstPaneSpan_i,
                      CWnd& wndSecondPane_i,
                      const DIRECTION_e dirSizingBarDirection_i,
                      const int nSizingBarSpan_i = DEFAULT_SIZING_BAR_SPAN );

	    const CWnd* GetPane( const PANE_e ePane_i ) const;
	    void SetSizingBarSpan( const int nSizingBarSpan_i );
        void SetSizingBarDirection( const DIRECTION_e ePaneDirection_i );
        LRESULT DoSizingBarDragging( WPARAM wMousePosition_i, LPARAM lUnUsed_i );

        // Create function for divider pane
        BOOL Create( const CRect& crWindowRect_i, 
                     CWnd* pParent_i,
                     const UINT uID_i,
                     const DWORD dwStyleWnd_i = WS_CHILD | WS_VISIBLE,
                     const DWORD dwStyleWndEx_i = WS_EX_CONTROLPARENT,
                     const DWORD dwStyleSizingBar_i = WS_CHILD | WS_VISIBLE,
                     const DWORD dwStyleSizingBarEx_i = 0 );

        CWnd* SetFocus();

        // Set's sizable state
        void SetSizable( const bool bIsSizable_i )
        {
            GetSizingBar().m_bIsSizable = bIsSizable_i;
        }

        // Returns true if sizable
        bool GetSizable() const
        {
            return GetSizingBar().m_bIsSizable;
        }

        // Overridden for handling insets.
        void GetClientRect( LPRECT lpRect_o );

        // Resize according to parent window size
        bool ResizeToFitParent( CRect* pParentRect_i );

        // Returns insets
        const CInsets& GetInsets(){ return m_inDividerInsets; }

        // Sets padding to be used
        void SetInsets( const CInsets& inPadding_i );

        // Returns constant reference to sizing bar
        const SizingBar& GetSizingBar() const
        {
            return m_SizingBar;
        }

        // Returns non constant reference to sizing bar
        SizingBar& GetSizingBar()
        {
            return m_SizingBar;
        }

        // Set's resizing mode
        void SetResizeMode( const RESIZE_MODE_e ermResizeMode_i )
        {
            m_ermResizeMode = ermResizeMode_i;
        }

        // Returns resizing mode
        RESIZE_MODE_e GetResizeMode()
        {
            return m_ermResizeMode;
        }

        // Returns validity of first pane's validity status
        bool IsFirstPaneValid()
        {
            return IsValidWindow( m_pwndFirstPane );
        }

        // Return second pane's validity status
        bool IsSecondPaneValid()
        {
            return IsValidWindow( m_pwndSecondPane );
        }

        // Class registration function
        static ATOM RegisterDividerWnd();

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(DividerWnd)
        afx_msg void OnSize( UINT uType_i, int nCx_i, int nCy_i );
        afx_msg BOOL OnEraseBkgnd( CDC* pDC_i );
	    //}}AFX_VIRTUAL

    protected:

        void InitLayout( const int nFirstPaneSpan_i, const int nSizingBarSpan_i );
        void HandleResize();
        bool HandleFirstPaneResize( const CRect& crNewClientRect_i );
        bool HandleSecondPaneResize( const CRect& crNewClientRect_i );
        bool HandleEqualResize( const CRect& crNewClientRect_i );

    private:

        // Disable external calls to CWnd version of Create and CreateEx
        // since we have our own version of create
        using CWnd::Create;
        using CWnd::CreateEx;

        SizingBar m_SizingBar;
        RESIZE_MODE_e m_ermResizeMode;

        // Insets used for padding
        CInsets m_inDividerInsets;
        
	    CWnd* m_pwndFirstPane;  // First pane window
	    CWnd* m_pwndSecondPane; // Second pane window

    	DECLARE_MESSAGE_MAP()
};// End Class DividerWnd
#endif // _DIVIDER_WND_H_