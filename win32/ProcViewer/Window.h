#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "INodeData.h"
#include "StyleParser.h"

class Window;

// Typedef for window list
typedef CList<Window*, Window*&> WindowList;
typedef CMap<HWND, HWND, Window*, Window*&> WindowMap;

// Clears a window data list
void ClearWindowDataList( WindowList& wlWindowList );

/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * Window - Denotes one window and ( if any ) it's child windows too.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-01
 */
class Window : public INodeData
{
    public:

        //************************************
        // Method:    Window -- Default constructor initializes members
        // FullName:  Window::Window
        // Access:    public 
        // Returns:   
        // Qualifier:
        //************************************
        Window() : m_dwThreadId( 0 ),
                   m_hWindow( 0 ),
                   m_dwStyle( 0 ),
                   m_dwStyleEx( 0 ),
                   m_dwId( 0 ),
                   m_hParent( 0 ),
                   m_bUnicode( FALSE ),
                   m_bEnabled( FALSE )
        {
            ZeroMemory( &m_wcWndClass, sizeof( m_wcWndClass ));
        }

        //************************************
        // Method:    ~Window : Destructor
        // FullName:  Window::~Window
        // Access:    public 
        // Returns:   
        // Qualifier:
        //************************************
        ~Window()
        {
            ClearWindowDataList( m_wlChildWindows );
        }

        //************************************
        // Method:    GetChildCount - Returns count of kids for this window
        // FullName:  Window::GetChildCount
        // Access:    public 
        // Returns:   UINT
        // Qualifier:
        //************************************
        UINT_PTR GetChildCount() const
        {
            return m_wlChildWindows.GetCount();
        }

        void ExtractWindowDetails( const HWND hWnd_i );

        //<TODO Description:- This function is meant to be removed>
        bool DeleteNodeData() const { return false; }

        //************************************
        // Method:    GetClass - Returns reference to window class
        // FullName:  Window::GetClass
        // Access:    public 
        // Returns:   WNDCLASSEX&
        // Qualifier:
        //************************************
        WNDCLASSEX& GetClass() { return m_wcWndClass; }
        //************************************
        // Method:    GetClass - Class name
        // FullName:  Window::GetClass
        // Access:    public 
        // Returns:   const WNDCLASSEX&
        // Qualifier: const
        //************************************
        const WNDCLASSEX& GetClass() const { return m_wcWndClass; }

        //************************************
        // Method:    GetHandle - Returns reference to window handle
        // FullName:  Window::GetHandle
        // Access:    public 
        // Returns:   HWND
        // Qualifier: const
        //************************************
        HWND GetHandle() const { return m_hWindow; }
        //************************************
        // Method:    SetHandle - Returns reference to window handle
        // FullName:  Window::SetHandle
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const HWND & hWnd_i
        //************************************
        void SetHandle( const HWND& hWnd_i ) { m_hWindow = hWnd_i; }

        //************************************
        // Method:    GetParentHandle - Returns reference to window handle
        // FullName:  Window::GetParentHandle
        // Access:    public 
        // Returns:   HWND
        // Qualifier: const
        //************************************
        HWND GetParentHandle() const { return m_hParent; }
        //************************************
        // Method:    SetParentHandle - Returns reference to window handle
        // FullName:  Window::SetParentHandle
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const HWND & hWnd_i
        //************************************
        void SetParentHandle( const HWND& hWnd_i ) { m_hParent = hWnd_i; }

        //************************************
        // Method:    GetClassName - Window class name
        // FullName:  Window::GetClassName
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetClassName() const { return m_csClassName; }
        //************************************
        // Method:    GetClassName - Window class name
        // FullName:  Window::GetClassName
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetClassName() { return m_csClassName; }

        //************************************
        // Method:    GetTitle -- Returns title of window
        // FullName:  Window::GetTitle
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetTitle() const { return m_csTitle; }
        //************************************
        // Method:    GetTitle - Returns title of window
        // FullName:  Window::GetTitle
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetTitle() { return m_csTitle; }


        //************************************
        // Method:    GetBounds - Window bounds
        // FullName:  Window::GetBounds
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetBounds() const { return m_csBounds; }
        //************************************
        // Method:    GetBounds - Window bounds
        // FullName:  Window::GetBounds
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetBounds() { return m_csBounds; }

        //************************************
        // Method:    GetState - Window state, maximized, minimized, restored, or invisible
        // FullName:  Window::GetState
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetState() const { return m_csState; }
        //************************************
        // Method:    GetState - Window state, maximized, minimized, restored, or invisible
        // FullName:  Window::GetState
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetState() { return m_csState; }

        //************************************
        // Method:    GetThreadId - Window thread id
        // FullName:  Window::GetThreadId
        // Access:    public 
        // Returns:   DWORD
        // Qualifier: const
        //************************************
        DWORD GetThreadId() const { return m_dwThreadId; }
        //************************************
        // Method:    SetThreadId
        // FullName:  Window::SetThreadId
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const DWORD dwThreadId
        //************************************
        void SetThreadId( const DWORD dwThreadId ) { m_dwThreadId = dwThreadId; }

        //************************************
        // Method:    GetStyle - Window style
        // FullName:  Window::GetStyle
        // Access:    public 
        // Returns:   DWORD
        // Qualifier: const
        //************************************
        DWORD GetStyle() const { return m_dwStyle; }
        //************************************
        // Method:    SetStyle - Window style
        // FullName:  Window::SetStyle
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const DWORD dwStyle
        //************************************
        void SetStyle( const DWORD dwStyle ){ m_dwStyle = dwStyle; }

        //************************************
        // Method:    GetStyleEx - Window style ex
        // FullName:  Window::GetStyleEx
        // Access:    public 
        // Returns:   DWORD
        // Qualifier: const
        //************************************
        DWORD GetStyleEx() const { return m_dwStyleEx; }
        //************************************
        // Method:    SetStyleEx - Window style ex
        // FullName:  Window::SetStyleEx
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const DWORD dwStyleEx
        //************************************
        void SetStyleEx( const DWORD dwStyleEx ) { m_dwStyleEx = dwStyleEx; }

        //************************************
        // Method:    GetId - Window id
        // FullName:  Window::GetId
        // Access:    public 
        // Returns:   DWORD
        // Qualifier: const
        //************************************
        DWORD GetId() const { return m_dwId; }
        //************************************
        // Method:    SetId - Window id
        // FullName:  Window::SetId
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const DWORD dwId
        //************************************
        void SetId( const DWORD dwId ) { m_dwId = dwId; }

        //************************************
        // Method:    GetIcon - Returns icon for a window
        // FullName:  Window::GetIcon
        // Access:    public 
        // Returns:   const Utils::AutoHICONMgr&
        // Qualifier: const
        //************************************
        const Utils::AutoHICONMgr& GetIcon() const { return m_ahmIcon; }
        //************************************
        // Method:    SetIcon - Set's icon for a window
        // FullName:  Window::SetIcon
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const HICON hIcon
        //************************************
        void SetIcon( const HICON hIcon ){ m_ahmIcon = hIcon; }

        //************************************
        // Method:    IsUnicode - Set/Get unicode/ansi status for window
        // FullName:  Window::IsUnicode
        // Access:    public 
        // Returns:   BOOL
        // Qualifier:
        //************************************
        BOOL IsUnicode() { return m_bUnicode; }
        //************************************
        // Method:    SetUnicode - Set/Get unicode/ansi status for window
        // FullName:  Window::SetUnicode
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const BOOL bUnicode
        //************************************
        void SetUnicode( const BOOL bUnicode ) { m_bUnicode = bUnicode; }

        //************************************
        // Method:    IsEnabled - Set/Get enabled/disabled status for window
        // FullName:  Window::IsEnabled
        // Access:    public 
        // Returns:   BOOL
        // Qualifier:
        //************************************
        BOOL IsEnabled() { return m_bEnabled; }
        //************************************
        // Method:    SetEnabled - Set/Get enabled/disabled status for window
        // FullName:  Window::SetEnabled
        // Access:    public 
        // Returns:   void
        // Qualifier:
        // Parameter: const BOOL bEnabled
        //************************************
        void SetEnabled( const BOOL bEnabled ) { m_bEnabled = bEnabled; }

        //************************************
        // Method:    GetChildWindowList - Child window list
        // FullName:  Window::GetChildWindowList
        // Access:    public 
        // Returns:   const WindowList&
        // Qualifier: const
        //************************************
        const WindowList& GetChildWindowList() const { return m_wlChildWindows; }
        //************************************
        // Method:    GetChildWindowList - Child window list
        // FullName:  Window::GetChildWindowList
        // Access:    public 
        // Returns:   WindowList&
        // Qualifier:
        //************************************
        WindowList& GetChildWindowList() { return m_wlChildWindows; }

        //************************************
        // Method:    GetClassStyleString
        // FullName:  Window::GetClassStyleString
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetClassStyleString() const { return m_csClassStyle; }
        //************************************
        // Method:    GetClassStyleString
        // FullName:  Window::GetClassStyleString
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetClassStyleString() { return m_csClassStyle; }

        //************************************
        // Method:    GetWindowStyleString
        // FullName:  Window::GetWindowStyleString
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetWindowStyleString() const { return m_csWindowStyle; }
        //************************************
        // Method:    GetWindowStyleString
        // FullName:  Window::GetWindowStyleString
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetWindowStyleString() { return m_csWindowStyle; }

        //************************************
        // Method:    GetWindowStyleExString
        // FullName:  Window::GetWindowStyleExString
        // Access:    public 
        // Returns:   const CString&
        // Qualifier: const
        //************************************
        const CString& GetWindowStyleExString() const { return m_csWindowStylEx; }
        //************************************
        // Method:    GetWindowStyleExString
        // FullName:  Window::GetWindowStyleExString
        // Access:    public 
        // Returns:   CString&
        // Qualifier:
        //************************************
        CString& GetWindowStyleExString() { return m_csWindowStylEx; }

        //************************************
        // Method:    GetStyleParser
        // FullName:  Window::GetStyleParser
        // Access:    public 
        // Returns:   const StyleParser&
        // Qualifier: const
        //************************************
        const StyleParser& GetStyleParser() const { return m_StyleParser; }

    protected:

        bool IsListControl() { return GetClassName().CompareNoCase( WC_LISTVIEW ) == 0; }
        bool IsComboExControl() { return GetClassName().CompareNoCase( WC_COMBOBOXEX ) == 0; }
        bool IsTabControl() { return GetClassName().CompareNoCase( WC_TABCONTROL ) == 0; }

        void RetrieveWndStyles();

        // Returns state of window
        void GetStateAsString( CString& csState_o ) const;

        // Callback function pointer
        typedef LPCTSTR ( Window::*PFN_STRING_FOR_STYLE )( const ULONG ulStyle_i, CString& csStyle_o ) const;

        // Convert style values to string.
        void ParseOutStyles( const ULONG ulStyle_i, 
                             CString& csStyle_o, 
                             const PFN_STRING_FOR_STYLE pfnStyleParser_i ) const;

        // Style parsers
        LPCTSTR GetStringForClassStyle( const ULONG ulStyle_i, CString& csStyle_o ) const;
        LPCTSTR GetStringForWindowStyle( const ULONG ulStyle_i ) const;
        LPCTSTR GetStringForWindowStyleEx( const ULONG ulStyle_i ) const;

    private:

        WNDCLASSEX  m_wcWndClass;

        HWND        m_hWindow;
        HWND        m_hParent;

        DWORD       m_dwThreadId;
        DWORD       m_dwStyle;
        DWORD       m_dwStyleEx;
        DWORD       m_dwId;

        Utils::AutoHICONMgr m_ahmIcon;

        BOOL m_bUnicode;
        BOOL m_bEnabled;

        // Child window list
        WindowList m_wlChildWindows;

        // Class name of window
        CString m_csClassName;
    
        // Title of the window
        CString m_csTitle;
    
        // Current state of window, ie whether maximized or minimized.
        CString m_csState;

        // Window bounds as string
        CString m_csBounds;

        // For window preview purpose
        CBitmap bmpWndPreview;
        CDC     dcWndDcCompatible;

        // Class style
        CString m_csClassStyle;

        // Window style
        CString m_csWindowStyle;

        // Window styleEx
        CString m_csWindowStylEx;

        // Helps in parsing out window styles, class styles, and control styles
        // common for all window's
        static StyleParser m_StyleParser;
};// End class Window

typedef Window* PWindow;

#endif //_WINDOW_H_