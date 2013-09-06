// ProcessViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessViewer.h"
#include "ProcessViewerDlg.h"
#include "DividerWnd.h"
#include "shlwapi.h"
#include "OptionsDlg.h"
#include "ProcessCollection.h"
#include "ProcessPrivilegeMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT TIMER_REFRESH_PROCESS_INFORMATION = 123;
const UINT TIMER_SET_STATUS_TEXT_DEDICATION = 124;
const UINT TIMER_LOAD_AUTOSTARUP_INFO = 125;

const UINT WAIT_TIME_FOR_SETTING_DEFAULT_TEXT = 4000;


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessViewerDlg dialog

CProcessViewerDlg::CProcessViewerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CProcessViewerDlg::IDD, pParent),
      m_hGDI32dll( 0 ),
      m_hAccel( 0 ),
      m_bSearchedInPath( false ),
      m_bLoadedDrivers( false )
{
    //{{AFX_DATA_INIT(CProcessViewerDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_ahmBigIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_ahmSmIcon = RCAST( HICON, 
                         LoadImage( AfxGetResourceHandle(), 
                                    MAKEINTRESOURCE( IDR_MAINFRAME ), 
                                    IMAGE_ICON, 
                                    GetSystemMetrics( SM_CXSMICON ), 
                                    GetSystemMetrics( SM_CYSMICON ), 
                                    LR_DEFAULTCOLOR ));

    DWORD dwComputerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
    VERIFY( GetComputerName( m_csComputerName.GetBuffer( dwComputerNameLength ), &dwComputerNameLength ));
    m_csComputerName.ReleaseBuffer();

    DWORD dwUserNameLength = MAX_PATH;
    GetUserName( m_csUserName.GetBuffer( dwUserNameLength ), &dwUserNameLength );
    m_csUserName.ReleaseBuffer();
}

CProcessViewerDlg::~CProcessViewerDlg()
{
    if( m_hGDI32dll )
    {
        FreeLibrary( m_hGDI32dll );
    }
}

void CProcessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProcessViewerDlg)
    DDX_Control(pDX, IDC_LIST_MODULESYMBOLS, m_clvModuleSymbolList);
    DDX_Control(pDX, IDC_PROGRESS_LOAD, m_PrgCtrl);
    DDX_Control(pDX, IDC_TREE_PROCESSDETAILS, m_ctvProcessDetails);
    DDX_Control(pDX, IDC_LIST_PROCESSMODULES, m_clvProcessModules);
    DDX_Control(pDX, IDC_TREE_PROCESS, m_ctvProcess);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcessViewerDlg, CDialog)
    //{{AFX_MSG_MAP(CProcessViewerDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESS, OnSelchangedTreeProcess)
    ON_WM_DESTROY()
    ON_COMMAND(ID_OPTIONS_REFRESH, OnOptionsRefresh)
    ON_COMMAND(ID_OPTIONS_SWAPLAYOUT, OnOptionsSwaplayout)
    ON_COMMAND(ID_OPTIONS_EXIT, OnOptionsExit)
    ON_COMMAND(ID_OPTIONS_ABOUT, OnOptionsAbout)
    ON_COMMAND(ID_OPTIONS_SHOWPATH, OnOptionsShowpath)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESSMODULES, OnDblclkListProcessdetails)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROCESS, OnDblclkTreeProcess)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_PROCESS, OnRClkTreeProcess)
    ON_COMMAND(ID_OPTIONS_ENABLEDEPENDS, OnOptionsEnabledepends)
    ON_COMMAND(ID_OPTIONS_SEARCH, OnOptionsSearch)
    ON_WM_TIMER()
    ON_COMMAND(ID_OPTIONS_SETTINGS, OnOptionsSettings)
    ON_COMMAND(ID_OPTIONS_KILLPROCESS, OnOptionsKillprocess)
    ON_COMMAND( ID_OPTIONS_KILLPROCESS_ALL_INSTANCES, OnOptionsKillprocessAllInstances )
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROCESSMODULES, OnItemchangedListProcessmodules)
    ON_NOTIFY(TVN_GETINFOTIP, IDC_TREE_PROCESS, OnGetInfoTipTreeProcess)
    ON_COMMAND_RANGE( ID_PRIORITY_REALTIME, ID_PRIORITY_LOW, HandleProcessPriorityCommand )
    ON_COMMAND( ID_PRIORITY_DISABLEBOOSTPRIORITY, HandleBoostPriorityCommand )
    ON_COMMAND( ID_UTILS_OPENPARENTFOLDER, HandleOpenParentFolder )
    ON_COMMAND( ID_UTILS_PROPERTIES, HandleProperties )
    ON_COMMAND( ID_UTILS_PRIVILEGEMANAGER, HandlePrivilegeManager )
	//}}AFX_MSG_MAP
    ON_NOTIFY_EX_RANGE( TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText )
    ON_NOTIFY_EX_RANGE( TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessViewerDlg message handlers

BOOL CProcessViewerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Load previous settings
    g_Settings.Load();

    // Move window to previous coords, since
    // divider will be using these co-oridnates for positioning it's divider
    WINDOWPLACEMENT wpPlacement = { 0 };
    wpPlacement.length = sizeof( wpPlacement );
    wpPlacement.rcNormalPosition = g_Settings.m_crMainWindowRect;
    wpPlacement.showCmd = SW_HIDE;
    ::SetWindowPlacement( GetSafeHwnd(), &wpPlacement );

    // Modify env path to add drivers path also
    ModifyEnvPATH();

    // Set up privileges
    SetUpAccess();

    CRect crClientRect;
    GetClientRect( &crClientRect );
    crClientRect.DeflateRect( 2, 2 );
    VERIFY( m_MainDivider.Create( crClientRect, this, 101010, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS ));
    m_clvProcessModules.SetParent( &m_MainDivider );
    m_MainDivider.SetResizeMode( RM_SECONDPANE );
    m_MainDivider.SetFocus();
    
    CRect crDividerRect;
    m_MainDivider.GetClientRect( &crDividerRect );
    m_ProcessDivider.Create( crDividerRect, &m_MainDivider, 12312, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS );
    m_ctvProcess.SetParent( &m_ProcessDivider );
    m_ctvProcessDetails.SetParent( &m_ProcessDivider );
    m_ProcessDivider.SetPane( m_ctvProcess, g_Settings.m_nProcessDividerSpan, m_ctvProcessDetails, DIR_HORZ );
    m_ProcessDivider.SetResizeMode( RM_SECONDPANE );

    m_ModuleDivider.Create( crDividerRect, &m_MainDivider, 19239, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS );
    m_clvProcessModules.SetParent( &m_ModuleDivider );
    m_clvModuleSymbolList.SetParent( &m_ModuleDivider );
    m_ModuleDivider.SetPane( m_clvProcessModules, g_Settings.m_nModuleDividerSpan, m_clvModuleSymbolList, DIR_HORZ );
    m_ModuleDivider.SetResizeMode( RM_SECONDPANE );

    if( !g_Settings.m_nDividerDirection )
    {
        g_Settings.m_nDividerDirection = DIR_VERT;
    }
    m_MainDivider.SetPane( m_ProcessDivider, 
                           g_Settings.m_nMainDividerSpan, 
                           m_ModuleDivider, 
                           SCAST( DIRECTION_e, g_Settings.m_nDividerDirection ));

    int nColumnIndex = 0;
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Index" ), LVCFMT_LEFT, 80, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Module name" ), LVCFMT_LEFT, 150, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Module path" ), LVCFMT_LEFT, 200, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Load address" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Image size" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Entry point" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Version" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Size" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Company" ), LVCFMT_LEFT, 200, nColumnIndex );
    m_clvProcessModules.InsertColumn( nColumnIndex++, _T( "Description" ), LVCFMT_LEFT, 300, nColumnIndex );

    nColumnIndex = 0;
    m_clvModuleSymbolList.InsertColumn( nColumnIndex++, _T( "Index" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvModuleSymbolList.InsertColumn( nColumnIndex++, _T( "Name" ), LVCFMT_LEFT, 150, nColumnIndex );
    m_clvModuleSymbolList.InsertColumn( nColumnIndex++, _T( "Address" ), LVCFMT_LEFT, 100, nColumnIndex );
    m_clvModuleSymbolList.InsertColumn( nColumnIndex++, _T( "Size" ), LVCFMT_LEFT, 100, nColumnIndex );
	m_clvModuleSymbolList.InsertColumn( nColumnIndex++, _T( "Type" ), LVCFMT_LEFT, 150, nColumnIndex );

    // Load previous state of list view control
    m_clvProcessModules.LoadState( _T( "ProcessModulesList" ));

    // Load previous state of list control
    m_clvModuleSymbolList.LoadState( _T( "ProcessSymbolsList" ));

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon( m_ahmBigIcon, TRUE );           // Set big icon
    SetIcon( m_ahmSmIcon, FALSE );      // Set small icon

    VERIFY( m_ImageList.Create( GetSystemMetrics( SM_CXSMICON ), 
                                GetSystemMetrics( SM_CYSMICON ), 
                                ILC_COLOR32 | ILC_MASK, 
                                0, 
                                20 ));
    VERIFY( m_Bitmap.LoadBitmap( IDB_BITMAP_IMAGES ));

    m_ImageList.Add( &m_Bitmap, RGB( 255, 0, 255 ));
    m_nInitialImageCount = m_ImageList.GetImageCount();
    m_ctvProcess.SetImageList( &m_ImageList, TVSIL_NORMAL );
    m_ctvProcessDetails.SetImageList( &m_ImageList, TVSIL_NORMAL );
    m_clvProcessModules.SetImageList( &m_ImageList, LVSIL_SMALL );
    m_clvModuleSymbolList.SetImageList( &m_ImageList, LVSIL_SMALL );
    m_clvProcessModules.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP );
    m_clvModuleSymbolList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP );

    m_hAccel = LoadAccelerators( AfxGetResourceHandle(), MAKEINTRESOURCE( IDR_ACCEL_MENU ));
    ASSERT( m_hAccel );

    SetupControlbars();

    m_ToolBar.GetToolBarCtrl().CheckButton( ID_OPTIONS_SHOWPATH, g_Settings.m_bIsFullPathPressed );
    m_ToolBar.GetToolBarCtrl().CheckButton( ID_OPTIONS_ENABLEDEPENDS, g_Settings.m_bIsDependsPressed );

    if( g_Settings.m_bIsDependsPressed )
    {
        SendMessage( WM_COMMAND, MAKEWPARAM( ID_OPTIONS_ENABLEDEPENDS, 0 ), 0 ); 
    }

    // Generate a full path message
    if( g_Settings.m_bIsFullPathPressed )
    {
        SendMessage( WM_COMMAND, MAKEWPARAM( ID_OPTIONS_SHOWPATH, 0 ), 0 ); 
    }

    // Load processes
    SetTimer( TIMER_REFRESH_PROCESS_INFORMATION, 0, 0 );
    UpdateModuleCount( 0 );

    m_ctvProcess.SetFocus();
    return FALSE;  // return TRUE  unless you set the focus to a control
}

void CProcessViewerDlg::OnDestroy()
{
    g_Settings.m_bIsFullPathPressed = m_ToolBar.GetToolBarCtrl().IsButtonChecked( ID_OPTIONS_SHOWPATH );
    g_Settings.m_bIsDependsPressed = m_ToolBar.GetToolBarCtrl().IsButtonChecked( ID_OPTIONS_ENABLEDEPENDS );
    g_Settings.m_nDividerDirection = m_MainDivider.GetSizingBar().GetDirection();

    CRect crProcessDividerRect;
    m_ProcessDivider.GetWindowRect( &crProcessDividerRect );

    if( g_Settings.m_nDividerDirection == DIR_HORZ )
    {
        g_Settings.m_nMainDividerSpan = crProcessDividerRect.Height();
    }
    else
    {
        g_Settings.m_nMainDividerSpan = crProcessDividerRect.Width();
    }// End if

    // Save span
    CRect crtvProcessRect;
    m_ctvProcess.GetWindowRect( &crtvProcessRect );
    g_Settings.m_nProcessDividerSpan = crtvProcessRect.Height();

    // Save span of module divider
    CRect crlvModuleListRect;
    m_clvProcessModules.GetWindowRect( &crlvModuleListRect );
    g_Settings.m_nModuleDividerSpan = crlvModuleListRect.Height();

    // Save state of process modules list view
    m_clvProcessModules.SaveState( _T( "ProcessModulesList" ));

    // Save state of symbol list
    m_clvModuleSymbolList.SaveState( _T( "ProcessSymbolsList" ));

    // Remove all items
    m_ctvProcess.DeleteAllItems();

    // We won't be saving maximized state
    // Get restored rectangle, in work area co-ordinates
    g_Settings.m_crMainWindowRect = Utils::GetRestoredSizeOfWindow( GetSafeHwnd() );

    // Save settings
    g_Settings.Save();
}// End OnDestroy


static UINT g_ToolIdArray[] = 
{
    ID_OPTIONS_REFRESH,
    ID_OPTIONS_SWAPLAYOUT,
    ID_OPTIONS_SHOWPATH,
    ID_OPTIONS_ENABLEDEPENDS,
    ID_OPTIONS_SEARCH,
    ID_OPTIONS_SETTINGS,
    ID_OPTIONS_KILLPROCESS,
    ID_SEPARATOR,
    ID_OPTIONS_ABOUT,
    ID_SEPARATOR,
    ID_OPTIONS_EXIT
};

static UINT g_StatusPanesArr[] = 
{
    ID_SP_COPYRIGHT,
    ID_SP_PROCESS_COUNT,
    ID_SP_MODULE_COUNT,
    ID_SP_DEDICATION,
    ID_SP_PERFORMANCE
};

void CProcessViewerDlg::SetupControlbars()
{
    CRect rcClientStart;
    GetClientRect(rcClientStart);

    m_StatusBar.Create( this );
    m_StatusBar.SetIndicators( g_StatusPanesArr, 
                               sizeof( g_StatusPanesArr ) / sizeof( g_StatusPanesArr[0] ));
    m_StatusBar.GetStatusBarCtrl().SetMinHeight( 23 );
    
    // Create the toolbar
    m_ToolBar.CreateEx( this, 
                        TBSTYLE_FLAT | TBSTYLE_TRANSPARENT,
                        WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_BORDER_BOTTOM );

    m_ToolBar.SetButtons( g_ToolIdArray, sizeof( g_ToolIdArray )/sizeof( g_ToolIdArray[0] ));
    m_ToolBar.SetSizes( CSize( 40, 40 ), CSize( 16, 16 ));

    CToolBarCtrl& tbcTbCtrl = m_ToolBar.GetToolBarCtrl();
    tbcTbCtrl.SetImageList( &m_ImageList );

    int nBtnIndex = 0;
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Refresh" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_REFRESH, TBSTYLE_FLAT, 4 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Layout" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_SWAPLAYOUT, TBSTYLE_FLAT, 6 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Full path" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_SHOWPATH, TBSTYLE_FLAT | TBBS_CHECKBOX , 53 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Depends" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_ENABLEDEPENDS, TBSTYLE_FLAT | TBBS_CHECKBOX , 10 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Search" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_SEARCH, TBSTYLE_FLAT, 40 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Options" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_SETTINGS, TBSTYLE_FLAT, 44 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Kill" ));
    m_ToolBar.SetButtonInfo( nBtnIndex++, ID_OPTIONS_KILLPROCESS, TBSTYLE_FLAT, 2 );
    m_ToolBar.SetButtonInfo( ++nBtnIndex, ID_OPTIONS_ABOUT, TBSTYLE_FLAT, 46 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "About" ));
    m_ToolBar.SetButtonInfo( nBtnIndex+=2, ID_OPTIONS_EXIT, TBSTYLE_FLAT, 45 );
    m_ToolBar.SetButtonText( nBtnIndex, _T( "Exit" ));

    // New client rectangle
    CRect rcClientNow;

    // To reposition and resize the control bar
    RepositionBars( AFX_IDW_CONTROLBAR_FIRST,
                    AFX_IDW_CONTROLBAR_LAST,
                    AFX_IDW_PANE_FIRST,
                    reposQuery,
                    rcClientNow );

    // Offset from top
    CPoint ptTopOffset( rcClientNow.left - rcClientStart.left,
                        rcClientNow.top - rcClientStart.top );

    // Offset from bottom
    CPoint ptBottomOffset( rcClientNow.left - rcClientStart.left,
                           rcClientStart.bottom - rcClientNow.bottom );
    
    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);

    while (pwndChild)
    {
        pwndChild->GetWindowRect( &rcChild );
        ScreenToClient( &rcChild );

        // Offset and resize child window rectangle for repositioning
        rcChild.OffsetRect( ptTopOffset );
        rcChild.DeflateRect( 0, 0, ptTopOffset.x + ptBottomOffset.x, ptTopOffset.y + ptBottomOffset.y );

        pwndChild->MoveWindow( &rcChild, FALSE );
        pwndChild = pwndChild->GetNextWindow();
    }// End while

    // And position the control bars
    RepositionBars( AFX_IDW_CONTROLBAR_FIRST, 
                    AFX_IDW_CONTROLBAR_LAST, 
                    AFX_IDW_PANE_FIRST );

    SetUpProgressBar();
}

BOOL CProcessViewerDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    // OnNeedText should only be called for TTN_NEEDTEXT notifications!
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

    // need to handle both ANSI and UNICODE versions of the message
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

    CString strTipText;

    UINT nID = pNMHDR->idFrom;
    if ( pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND ) ||
         pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND ))
    {
        // idFrom is actually the HWND of the tool
        nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
    }

    if (nID != 0) // will be zero on a separator
    {
        strTipText.LoadString( nID );
    }

    // Handle conditionally for both UNICODE and non-UNICODE apps
    #ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
            lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
        else
            _mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
    #else
        if (pNMHDR->code == TTN_NEEDTEXTA)
            _wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
        else
            lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
    #endif
    *pResult = 0;

    // Bring to front
    ::SetWindowPos( pNMHDR->hwndFrom, 
                    HWND_TOP, 
                    0, 
                    0, 
                    0, 
                    0,
                    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE );

    return TRUE;    // message was handled
}

void CProcessViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessViewerDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_ahmSmIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessViewerDlg::OnQueryDragIcon()
{
    return (HCURSOR)(HICON) m_ahmSmIcon;
}


void CProcessViewerDlg::RefreshProcessInformation( PSEARCH_CRITERIA pscCriteria_i )
{
    // Clear previous associated item data if any
    UpdateCount();

    DWORD dwSize = 0;
    DWORD dwProcIdentifiers[MAX_MODULES] = { 0 };

	SetTimer( TIMER_LOAD_AUTOSTARUP_INFO, 1000, 0 );

    // Start performance measurement
    BeginPerformance();

    // Load all processes
    EnumProcesses( dwProcIdentifiers, MAX_MODULES, &dwSize );
    if( !dwSize )
    {
        return;
    }

    // Unset flag
    m_bLoadedDrivers = false;

    // Process count
    dwSize /= sizeof( DWORD );

    m_PrgCtrl.ShowWindow( SW_SHOWNA );
    ReInitProgressBar( ( int )dwSize );\

    // Tell progress
    UpdateProgressText( _T( "Loading process information..." ));

    // Redraw management object
    Utils::AutoSetRedrawManager asrmRedrMgr;
    asrmRedrMgr.AddWindow( m_ctvProcess );
    asrmRedrMgr.AddWindow( m_ctvProcessDetails );
    asrmRedrMgr.AddWindow( m_clvProcessModules );
    asrmRedrMgr.AddWindow( m_clvModuleSymbolList );

    // Clear tree view
    m_ctvProcess.DeleteAllItems();

    // Clear process modules list
    m_clvProcessModules.DeleteAllItems();

    // Clears process details tree
    m_ctvProcessDetails.DeleteAllItems();

    UpdateProgressText( _T( "Loading window details..." ));

    // Get all windows
	if( g_Settings.m_bProcessHwnd )
	{
		m_WC.Enumerate();
	}

    // Clears module symbol information list
    m_clvModuleSymbolList.DeleteAllItems();

    const int nNewImageCount = m_ImageList.GetImageCount();
    for( int nIndex = nNewImageCount; nIndex > m_nInitialImageCount; --nIndex )
    {
        VERIFY( m_ImageList.Remove( nIndex - 1 ));
    }

    POSITION pstPos = m_lstExtraIcons.GetHeadPosition();
    while( pstPos )
    {
        ::DestroyIcon( m_lstExtraIcons.GetNext( pstPos ));
    }

    // Remove all entries
    m_lstExtraIcons.RemoveAll();

    // Insert a default item as root
    HTREEITEM hItemRoot = m_ctvProcess.InsertItemEx( CString( _T( "\\\\" )) + m_csComputerName + _T( "\\" ) + m_csUserName, 60, 60 );

    // Loop through each process
    for( DWORD dwIndex = 0; dwIndex < dwSize; ++dwIndex )
    {
        // Update progress
        UpdateProgressBar( dwIndex + 1 );
        
        // Flush out any pending events, simulating a thread, so that UI doesn't block
        Utils::DoEvents( GetSafeHwnd() );

        // Structure for storing procedure information
        PPROC_INFO_t pstProcInfo = new PROC_INFO_t;
        if( !pstProcInfo )
        {
            continue;
        }

        // Get process information
        if( !GetProcessDetail( dwProcIdentifiers[dwIndex], *pstProcInfo, pscCriteria_i ))
        {
            // Delete process information
            delete pstProcInfo;

            // Skip rest since some problem happened
            continue;
        }

        // Get full path display status, if checked then we will display full path
        const BOOL bFullPathChecked = m_ToolBar.GetToolBarCtrl().IsButtonChecked( ID_OPTIONS_SHOWPATH );

        CString csProc;
        csProc.Format( _T( "%s - %lu" ), SCAST( LPCTSTR , ( bFullPathChecked ? pstProcInfo->csFullPath : pstProcInfo->csBaseName )), 
                                         dwProcIdentifiers[dwIndex] );

        // Get icon for file type
        HICON hIcon = GetAssociatedFileIcon( pstProcInfo->csFullPath );
        int nIndex = 20; 
        if( hIcon  )
        {
            nIndex = m_ImageList.Add( hIcon );
            if( nIndex  == -1 )
            {
                ASSERT( FALSE );
                nIndex = 20;
            }
        }// End if

        // Get exe type
        GetExeType( pstProcInfo->csFullPath, pstProcInfo->csApplicationType );

        // Insert process node into tree
        m_ctvProcess.InsertItemEx( csProc, 
                                 nIndex, 
                                 nIndex, 
                                 IT_PROC_INFO, 
                                 pstProcInfo,
                                 hItemRoot );
    }// End for

    // Update count of processes
    UpdateProcessCount( m_ctvProcess.GetImmediateChildrenCount( m_ctvProcess.GetRootItem() ));

    // Expand
    m_ctvProcess.Expand( hItemRoot, TVE_EXPAND );
    UpdateProgressText( _T( "Done" ));
    UpdateProgressText( 0 );
    m_PrgCtrl.ShowWindow( SW_HIDE );

    // Display performance
    EndPerformance();
}// End RefreshProcessInformation

HANDLE GetProcessHandle( const DWORD dwProcessId_i )
{
   // Get process handle
   Utils::AutoHandleMgr ahmProcess( OpenProcess( PROCESS_ALL_ACCESS, 
                                                 FALSE, 
                                                 dwProcessId_i ));

   // Did we get the the handler
   if( !ahmProcess )
   {
        // Open process with limited read access
        ahmProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_DUP_HANDLE, 
                                  FALSE, 
                                  dwProcessId_i );
        if( !ahmProcess )
        {
            return 0;
        }
   }// End if

   Utils::AutoHandleMgr ahmDuplicateProcHandle;
   VERIFY( DuplicateHandle( GetCurrentProcess(), 
                            ahmProcess, 
                            GetCurrentProcess(), 
                            &ahmDuplicateProcHandle.m_hHandle,
                            PROCESS_ALL_ACCESS,
                            FALSE,
                            0 ));

   return ahmDuplicateProcHandle.Detach();
}

bool CProcessViewerDlg::GetProcessDetail( const DWORD dwProcessID_i, 
                                          PROC_INFO_t& stpiProcInfo_i,  
                                          PSEARCH_CRITERIA pSrchCriteria_i )
{
    // Module array
    HMODULE hModules[MAX_MODULES] = { 0 };

    // Name variable
    TCHAR szName[MAX_PATH] = { 0 };

    Utils::AutoHandleMgr ahmProcess( GetProcessHandle( dwProcessID_i ));
    if( !ahmProcess )
    {
       return false;
    }

    // Load module information for a process
    if( GetProcessModulesInfo( ahmProcess,
                               stpiProcInfo_i,
                               hModules, 
                               pSrchCriteria_i ))
    {
        // Get module full file name
        GetModuleFileNameEx( ahmProcess, hModules[0], szName, MAX_PATH );
        stpiProcInfo_i.csFullPath = szName;

        // Get module base name
        GetModuleBaseName( ahmProcess, hModules[0], szName, MAX_PATH );
        stpiProcInfo_i.csBaseName = szName;
    }
    // Since modules were not found, then it could be device drivers, try for this
    else if( !m_bLoadedDrivers )
    {
        if( !GetDeviceDrivers( stpiProcInfo_i,
                               pSrchCriteria_i ))
        {
            return false;
        }// End if
    }
    else
    {
        return false;
    }

    
    // Print progress message
    CString csProgress;
    csProgress.Format( _T( "%s: %s" ), 
                       ( pSrchCriteria_i ? _T( "Searching" ) : _T( "Loading" )), 
                         stpiProcInfo_i.csBaseName );
    UpdateProgressText( csProgress );

    // Fill process id
    stpiProcInfo_i.dwProcId = dwProcessID_i;

    // Update search type
    if( pSrchCriteria_i && ST_PROCESS == pSrchCriteria_i->eSrchType )
    {
        if( !StrStrI( stpiProcInfo_i.csBaseName, pSrchCriteria_i->csSrch ))
        {
            return false;
        }// End if
    }// End if

    // Check if we have valid path or not
    if( stpiProcInfo_i.csFullPath[1] != _T( ':' ))
    {
        Utils::FindFile( stpiProcInfo_i.csBaseName, stpiProcInfo_i.csFullPath );
    }

    // Set version information
    stpiProcInfo_i.fviVersion.SetFilePath( stpiProcInfo_i.csFullPath );

    // Detach process handle and store
    stpiProcInfo_i.ahmProcessHandle = ahmProcess.Detach();
    
    return true;
}


bool CProcessViewerDlg::GetProcessModulesInfo( HANDLE& hProcess_i,
                                               PROC_INFO_t& stProcInfo_i,
                                               HMODULE* hProcModuleArray_i, 
                                               PSEARCH_CRITERIA pSrchCriteria_i )

{
    if( !hProcess_i )
    {
        return false;
    }

    // Name variable
    TCHAR szName[MAX_PATH] = { 0 };
    DWORD dwModuleSize = 0;

    dwModuleSize = 0;

    // Get all modules for a process
    EnumProcessModules( hProcess_i, hProcModuleArray_i, MAX_MODULES, &dwModuleSize );
    if( !dwModuleSize )
    {
        return false;
    }

    dwModuleSize /= sizeof( HMODULE );

    // Store process module handle
    stProcInfo_i.hProcessModule = hProcModuleArray_i[0];

    // Extract information for process
    GetModuleInformation( hProcess_i, 
                          hProcModuleArray_i[0], 
                          &stProcInfo_i.stmiModInfo, 
                          sizeof( stProcInfo_i.stmiModInfo ));

    // Prepare module list
    PMODULEINFO_LIST_t* ppModuleInfo = &stProcInfo_i.pstModuleInfoList;
    
    bool bDLLFound = false;
    for( DWORD dwModIndex = 1; dwModIndex < dwModuleSize; ++ dwModIndex )
    {
        // Allocate new module
        *ppModuleInfo = new MODULEINFO_LIST_t;
        if( !*ppModuleInfo )
        {
            continue;
        }

        // Get module related information
        GetModuleInformation( hProcess_i, 
                              hProcModuleArray_i[dwModIndex], 
                              &( *ppModuleInfo )->stmiModInfo, 
                              sizeof( MODULEINFO ));
    
        // Get base name of module
        GetModuleBaseName( hProcess_i, hProcModuleArray_i[dwModIndex], szName, MAX_PATH );
        ( *ppModuleInfo )->csModuleBaseName = szName;
    
        // Check search criteria
        if( pSrchCriteria_i && ST_DLL == pSrchCriteria_i->eSrchType )
        {
            // Ignore all unmatched dlls
            if( !StrStrI( ( *ppModuleInfo )->csModuleBaseName , pSrchCriteria_i->csSrch ))
            {
                delete *ppModuleInfo;
                *ppModuleInfo = 0;

                continue;
            }
        }// End if

        // Found one
        bDLLFound = true;
    
        // Get full path of module
        GetModuleFileNameEx( hProcess_i, hProcModuleArray_i[dwModIndex], szName, MAX_PATH );
        ( *ppModuleInfo )->csModuleFullPath = szName;
    
        FileVersionInfo& fiInfo = ( *ppModuleInfo )->fviVersion;
        fiInfo.SetFilePath( szName );

        // Move forward
        ppModuleInfo = &( *ppModuleInfo )->pstNextModuleInfo;
    }// End for
   
    // If a search was going on the tell that we didn't find a dll if condition is 
    // satisfied
    return ( !pSrchCriteria_i || ST_DLL != pSrchCriteria_i->eSrchType || bDLLFound );
}


bool CProcessViewerDlg::GetDeviceDrivers( PROC_INFO_t& stProcInfo_i,
                                          PSEARCH_CRITERIA pSrchCriteria_i )
{
    // Set flag 
    m_bLoadedDrivers = true;
    
    TCHAR szName[MAX_PATH] = { 0 };
    DWORD dwModuleSize = 0;

    // Load address array
    LPVOID lpvdLoadAddresses[MAX_MODULES] = { 0 };

    // Must be "system"
    EnumDeviceDrivers( lpvdLoadAddresses, MAX_MODULES, &dwModuleSize );
    if( !dwModuleSize )
    {
        return false;
    }

    // Module size
    dwModuleSize /= sizeof( LPVOID );

    // Get device driver full file name
    GetDeviceDriverFileName( lpvdLoadAddresses[0], szName, MAX_PATH );
    stProcInfo_i.csFullPath = szName;

    // Just get the base name
    GetDeviceDriverBaseName( lpvdLoadAddresses[0], szName, MAX_PATH );
    stProcInfo_i.csBaseName = szName;

    // Prepare module list
    PMODULEINFO_LIST_t* ppModuleInfo = &stProcInfo_i.pstModuleInfoList;
 
    // DLL found flag
    bool bDLLFound = false;

    // Ignore first entry
    for( DWORD dwDriverIndex = 1; dwDriverIndex < dwModuleSize; ++dwDriverIndex )
    {
        // Allocate new module
        *ppModuleInfo = new MODULEINFO_LIST_t;;
        if( !*ppModuleInfo )
        {
            continue;
        }

        // Set driver information
        ( *ppModuleInfo )->stmiModInfo.lpBaseOfDll = lpvdLoadAddresses[dwDriverIndex];

        // Get base name of driver
        GetDeviceDriverBaseName( lpvdLoadAddresses[dwDriverIndex], szName, MAX_PATH );
        ( *ppModuleInfo )->csModuleBaseName = szName;

        // If a search is going then check to see if this driver satisfies the search 
        // criteria
        if( pSrchCriteria_i && ST_DLL == pSrchCriteria_i->eSrchType )
        {
            // Ignore all unmatched drivers
            if( !StrStrI( ( *ppModuleInfo )->csModuleBaseName , pSrchCriteria_i->csSrch ))
            {
                delete *ppModuleInfo;
                *ppModuleInfo = 0;

                // Skip reset
                continue;
            }
        }// End if

        // Found one
        bDLLFound = true;

        // Hold file version
        FileVersionInfo& fiInfo = ( *ppModuleInfo )->fviVersion;

        // Find out where the driver is located
        if( Utils::FindFile( szName, ( *ppModuleInfo )->csModuleFullPath ))
        {
            fiInfo.SetFilePath( ( *ppModuleInfo )->csModuleFullPath );
        }
        else
        {
            GetDeviceDriverFileName( lpvdLoadAddresses[dwDriverIndex], szName, MAX_PATH );
            ( *ppModuleInfo )->csModuleFullPath = szName;

            // Last ditch attempt to get file name
            ParseOutFileName(( *ppModuleInfo )->csModuleFullPath );

            // Set path
            fiInfo.SetFilePath(( *ppModuleInfo )->csModuleFullPath );
        }// End if

        // Move forward
        ppModuleInfo = &( *ppModuleInfo )->pstNextModuleInfo;
    }// End for

    // If a search was going on then tell that we didn't find a dll if condition is 
    // not satisfied
    return ( !pSrchCriteria_i || ST_DLL != pSrchCriteria_i->eSrchType || bDLLFound );
}// End GetDeviceDrivers


void CProcessViewerDlg::LoadWindowDetailsForProcess( PROC_INFO_t& stpiProcInfo_i )
{
    // Insert main window caption
    PProcessWindowCollection pProcWindowData = m_WC.GetProcessWindowCollection( stpiProcInfo_i.dwProcId );
    if( pProcWindowData )
    {
        const WindowList& wlWindowList = pProcWindowData->GetProcessWindowList();
        CString csTempFormatStr;
        csTempFormatStr.Format( _T( "Windows: Top level: %lu, Total: %d" ), pProcWindowData->GetProcessWindowCount(), pProcWindowData->GetTotalWindowCount() );
        HTREEITEM hWindowDetails = m_ctvProcessDetails.InsertItemEx( csTempFormatStr, 57, 57 );
        if( hWindowDetails )
        {
            // Get head position
            POSITION pstPos = wlWindowList.GetHeadPosition();

            // While position is valid
            while( pstPos )
            {
                // Get next window
                PWindow pWindowData = wlWindowList.GetNext( pstPos );
                AddWindowDetailsToTree( hWindowDetails, pWindowData );
            }// End while

            // Expand
            m_ctvProcessDetails.Expand( hWindowDetails, TVE_EXPAND );
        }// End if
    }// End if
}// End LoadWindowDetailsForProcess

// Adds window details to tree
void CProcessViewerDlg::AddWindowDetailsToTree( HTREEITEM hItem, PWindow pWindow_i )
{
    if( !pWindow_i )
    {
        return;
    }

    // Default icon index
    const int nIconIndex = 63;

    // String for inserting elements to tree
    CString csTempFormatStr;

    // Insert HWND
    csTempFormatStr.Format( _T( "HWND: 0x%p, [Class: %s, Text: %s]" ), pWindow_i->GetHandle(), 
                                                                       pWindow_i->GetClassName(), 
                                                                       pWindow_i->GetTitle() );
    HTREEITEM hHwndItem = m_ctvProcessDetails.InsertItemEx( csTempFormatStr, 
                                                          nIconIndex, 
                                                          nIconIndex, 
                                                          IT_WINDOW_INFO, 
                                                          pWindow_i, 
                                                          hItem );

    // Insert thread id
    csTempFormatStr.Format( _T( "Thread id: %lu" ), pWindow_i->GetThreadId() );
    m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );

    // Get class styles as string
    if( !pWindow_i->GetClassStyleString().IsEmpty() )
    {
        csTempFormatStr.Format( _T( "Class style: %s" ), SCAST( LPCTSTR, pWindow_i->GetClassStyleString() ));
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    // Insert window id
    if( pWindow_i->GetId() )
    {
        csTempFormatStr.Format( _T( "ID: %lu" ), pWindow_i->GetId() );
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    // Get window styles as string
    if( pWindow_i->GetStyle() )
    {
       csTempFormatStr.Format( _T( "Win style: %s" ), SCAST( LPCTSTR, pWindow_i->GetWindowStyleString() ));
       m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    // Get window style ex as string
    if( pWindow_i->GetStyleEx() )
    {
       csTempFormatStr.Format( _T( "WinEx style: %s" ), SCAST( LPCTSTR, pWindow_i->GetWindowStyleExString() ));
       m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    if( pWindow_i->GetParentHandle() )
    {
        csTempFormatStr.Format( _T( "Parent: 0x%08p" ), pWindow_i->GetParentHandle() );
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    if( pWindow_i->GetClass().lpfnWndProc )
    {
        csTempFormatStr.Format( _T( "Window proc: 0x%p" ), pWindow_i->GetClass().lpfnWndProc );
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    if( !pWindow_i->GetBounds().IsEmpty() )
    {
        csTempFormatStr.Format( _T( "Bounds: %s" ), SCAST( LPCTSTR, pWindow_i->GetBounds() ));
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    if( !pWindow_i->GetState().IsEmpty() )
    {
        csTempFormatStr.Format( _T( "State: %s" ), SCAST( LPCTSTR, pWindow_i->GetState() ));
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );
    }

    // Charset of a window
    csTempFormatStr.Format( _T( "Charset: %s" ), 
                            ( pWindow_i->IsUnicode() ? _T( "Unicode" ) : _T( "ANSI" )));
    m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );

    // Enabled/Disabled state of a window
    csTempFormatStr.Format( _T( "%s" ), 
                            ( pWindow_i->IsEnabled() ? _T( "Enabled" ) : _T( "Disabled" )));
    m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIconIndex, nIconIndex, hHwndItem );

    const HICON& hWndIcon = pWindow_i->GetIcon();
    if( hWndIcon )
    {
        csTempFormatStr.Format( _T( "Small icon" ));
        int nIndex = m_ImageList.Add( hWndIcon );
        nIndex = nIndex != -1 ? nIndex : nIconIndex;
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIndex, nIndex, hHwndItem );
    }

    if( pWindow_i->GetClass().hCursor )
    {
        csTempFormatStr.Format( _T( "Cursor" ));
        m_lstExtraIcons.AddTail( pWindow_i->GetClass().hCursor );
        int nIndex = m_ImageList.Add( pWindow_i->GetClass().hCursor );
        nIndex = nIndex != -1 ? nIndex : nIconIndex;
        m_ctvProcessDetails.InsertItemEx( csTempFormatStr, nIndex, nIndex, hHwndItem );
    }

    // Get child window list
    const WindowList& wlWndDataList = pWindow_i->GetChildWindowList();

    // Loop through child windows if any
    POSITION pstPos = wlWndDataList.GetHeadPosition();
    if( pstPos )
    {
        csTempFormatStr.Format( _T( "Child windows: %lu" ), pWindow_i->GetChildCount() );
        HTREEITEM hChildWindows = m_ctvProcessDetails.InsertItemEx( csTempFormatStr, 62, 62, hHwndItem );
        while( pstPos )
        {
            PWindow pWindow = wlWndDataList.GetNext( pstPos );
            AddWindowDetailsToTree( hChildWindows, pWindow );
        }// End while
        m_ctvProcessDetails.Expand( hChildWindows, TVE_EXPAND );
    }
}// End AddWindowDetailsToTree


bool CProcessViewerDlg::PrepareGDIInfoFunction()
{
    m_hGDI32dll = LoadLibrary( _T( "GDI32.dll" ));
    if( !m_hGDI32dll )
    {
        ASSERT( FALSE );
        return false;
    }

    if( m_hGDI32dll )
    {
        m_GDIFunc = RCAST( PFN_GDI_QUERY_TABLE_FUNCTION, 
                           GetProcAddress( m_hGDI32dll, "GdiQueryTable" ));
        if( !m_GDIFunc )
        {
            ASSERT( FALSE );
            return false;
        }
    }

    return true;
}


void CProcessViewerDlg::GetProcessTimes( PROC_INFO_t& stpiProcInfo_o )
{
    // Get file times for process
    FILETIME ftStartTime = { 0 },
             ftExitTime = { 0 },
             ftKernelModeTime = { 0 },
             ftUserModeTime = { 0 };

    ::GetProcessTimes( stpiProcInfo_o.ahmProcessHandle, &ftStartTime, &ftExitTime, &ftKernelModeTime, &ftUserModeTime );
    Utils::GetFormattedTime( stpiProcInfo_o.csProcessStartTime, ftStartTime, true );
    Utils::GetFormattedTime( stpiProcInfo_o.csExitTime, ftExitTime, true );
    Utils::GetFormattedTime( stpiProcInfo_o.csKernelTime, ftKernelModeTime, false );
    Utils::GetFormattedTime( stpiProcInfo_o.csUserTime, ftUserModeTime, false );
}

void CProcessViewerDlg::GetProcessPriority( HANDLE& hProcess_i, 
                                            CString& csPriority_o,
                                            UINT& uProcessPriority_o )
{
    uProcessPriority_o = GetPriorityClass( hProcess_i );
    switch( uProcessPriority_o )
    {
    case NORMAL_PRIORITY_CLASS:
        csPriority_o = _T( "Priority: Normal" );
        break;
    case ABOVE_NORMAL_PRIORITY_CLASS:
        csPriority_o = _T( "Priority: Above normal" );
        break;
    case BELOW_NORMAL_PRIORITY_CLASS:
        csPriority_o = _T( "Priority: Below normal" );
        break;
    case HIGH_PRIORITY_CLASS:
        csPriority_o = _T( "Priority: High" );
        break;
    case REALTIME_PRIORITY_CLASS:
        csPriority_o = _T( "Priority: Realtime" );
        break;
    case IDLE_PRIORITY_CLASS:
        csPriority_o = _T( "Priority: Idle" );
        break;
    }// End switch
}// End GetProcessPriority

void CProcessViewerDlg::GetProcessIOCounters( PROC_INFO_t& stpiProcInfo_i )
{
    IO_COUNTERS sticCounter = { 0 };
    if( !::GetProcessIoCounters( stpiProcInfo_i.ahmProcessHandle, &sticCounter ))
    {
        TRACE_ERR( "Failed to get proces IO counters" );
        return;
    }

    // Get information
    stpiProcInfo_i.csReadOprCount.Format( _T( "Read operation: %I64d" ), sticCounter.ReadOperationCount );
    stpiProcInfo_i.csWriteOprCount.Format( _T( "Write operation: %I64d" ), sticCounter.WriteOperationCount );
    stpiProcInfo_i.csOtherOprCount.Format( _T( "Other operation: %I64d" ), sticCounter.OtherOperationCount );
    stpiProcInfo_i.csReadTrnsCount.Format( _T( "Read transfer: %I64d" ), sticCounter.ReadTransferCount );
    stpiProcInfo_i.csWriteTrnsCount.Format( _T( "Write transfer: %I64d" ), sticCounter.WriteTransferCount );
    stpiProcInfo_i.csOtherTrnsCount.Format( _T( "Other transfer: %I64d" ), sticCounter.OtherTransferCount );
}

void CProcessViewerDlg::LoadProcessTimesForProcess( PROC_INFO_t& stpiProcInfo_i )
{
    const int nIconIndex = 63;

    // Get process times
    GetProcessTimes( stpiProcInfo_i );

    // Insert process time to tree
    HTREEITEM hProcessTimes = m_ctvProcessDetails.InsertItemEx( _T( "Process times" ), 39, 39 );
    if( hProcessTimes )
    {
        CString csTime;
        csTime.Format( _T( "Start time: %s" ), stpiProcInfo_i.csProcessStartTime );
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hProcessTimes );

        csTime.Format( _T( "Exit time: %s" ), stpiProcInfo_i.csExitTime );
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hProcessTimes );

        csTime.Format( _T( "Kernel mode time: %s" ), stpiProcInfo_i.csKernelTime );
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hProcessTimes );

        csTime.Format( _T( "User mode time: %s" ), stpiProcInfo_i.csUserTime );
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hProcessTimes );
        m_ctvProcessDetails.Expand( hProcessTimes, TVE_EXPAND );
    }// End if
}// End LoadProcessTimesForProcess

void CProcessViewerDlg::LoadFileTimesForProcess( PROC_INFO_t& stpiProcInfo_i )
{
    const int nIconIndex = 63;

    // Insert file times
    HTREEITEM hFileTimes = m_ctvProcessDetails.InsertItemEx( _T( "File times" ), 39, 39 );
    if( hFileTimes )
    {
        const CFileStatus& cfStatus = stpiProcInfo_i.fviVersion.GetFileStatus();

        CString csTime = cfStatus.m_ctime.Format( _T( "Created on: %A, %B %d, %Y - %H:%M:%S" ));
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hFileTimes );

        csTime = cfStatus.m_atime.Format( _T( "Accessed on: %A, %B %d, %Y - %H:%M:%S" ));
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hFileTimes );

        csTime = cfStatus.m_mtime.Format( _T( "Last written on: %A, %B %d, %Y - %H:%M:%S" ));
        m_ctvProcessDetails.InsertItemEx( csTime, nIconIndex, nIconIndex, hFileTimes );
        m_ctvProcessDetails.Expand( hFileTimes, TVE_EXPAND );
    }// End if
}// End LoadFileTimesForProcess

void CProcessViewerDlg::LoadIOCounters( PROC_INFO_t& stpiProcInfo_i )
{
    const int nIconIndex = 63;

    // Load IO counters for process
    GetProcessIOCounters( stpiProcInfo_i );

    HTREEITEM hIOCounterItem = m_ctvProcessDetails.InsertItemEx( _T( "IO Counters" ), 50, 50 );
    if( !stpiProcInfo_i.csReadOprCount.IsEmpty() )
    {
        m_ctvProcessDetails.InsertItemEx( stpiProcInfo_i.csReadOprCount, nIconIndex, nIconIndex, hIOCounterItem );
    }

    if( !stpiProcInfo_i.csWriteOprCount.IsEmpty() )
    {
        m_ctvProcessDetails.InsertItemEx( stpiProcInfo_i.csWriteOprCount, nIconIndex, nIconIndex, hIOCounterItem );
    }

    if( !stpiProcInfo_i.csOtherOprCount.IsEmpty() )
    {
        m_ctvProcessDetails.InsertItemEx( stpiProcInfo_i.csOtherOprCount, nIconIndex, nIconIndex, hIOCounterItem );
    }

    if( !stpiProcInfo_i.csReadTrnsCount.IsEmpty() )
    {
        m_ctvProcessDetails.InsertItemEx( stpiProcInfo_i.csReadTrnsCount, nIconIndex, nIconIndex, hIOCounterItem );
    }

    if( !stpiProcInfo_i.csWriteTrnsCount.IsEmpty() )
    {
        m_ctvProcessDetails.InsertItemEx( stpiProcInfo_i.csWriteTrnsCount, nIconIndex, nIconIndex, hIOCounterItem );
    }

    if( !stpiProcInfo_i.csOtherTrnsCount.IsEmpty() )
    {
        m_ctvProcessDetails.InsertItemEx( stpiProcInfo_i.csOtherTrnsCount, nIconIndex, nIconIndex, hIOCounterItem );
    }

    // Expand node
    m_ctvProcessDetails.Expand( hIOCounterItem, TVE_EXPAND );
}

void CProcessViewerDlg::LoadMemoryDetailsForProcess( PROC_INFO_t& stpiProcInfo_i )
{
    const int nIconIndex = 63;

    // Process details
    PROCESS_MEMORY_COUNTERS& pmcProcMemCounter = stpiProcInfo_i.stpmcMemCounters;
    GetProcessMemoryInfo( stpiProcInfo_i.ahmProcessHandle, &pmcProcMemCounter, sizeof( pmcProcMemCounter ));

    // Insert process memory related values
    HTREEITEM hPInfoItem = m_ctvProcessDetails.InsertItemEx( _T( "Memory details" ), 7, 7 );

    if( hPInfoItem )
    {
        CString csTempStr;
    
        // Page fault count
        csTempStr.Format( _T( "Page fault count: %lu" ), pmcProcMemCounter.PageFaultCount );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Page file usage
        csTempStr.Format( _T( "Page file usage: %.02lf KB" ), pmcProcMemCounter.PagefileUsage/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Peak page file usage
        csTempStr.Format( _T( "Peak page file usage: %.02lf KB" ), pmcProcMemCounter.PeakPagefileUsage/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Peak working set size
        csTempStr.Format( _T( "Peak working set size: %.02lf KB" ), pmcProcMemCounter.PeakWorkingSetSize/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Quota non pages pool usage
        csTempStr.Format( _T( "Quota non paged pool size: %.02lf KB" ), pmcProcMemCounter.QuotaNonPagedPoolUsage/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Quota pages pool usage
        csTempStr.Format( _T( "Quota paged pool size: %.02lf KB" ), pmcProcMemCounter.QuotaPagedPoolUsage/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Quota peak non pages pool usage
        csTempStr.Format( _T( "Quota peak non paged pool size: %.02lf KB" ), pmcProcMemCounter.QuotaPeakNonPagedPoolUsage/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Quota peak pages pool usage
        csTempStr.Format( _T( "Quota peak paged pool size: %.02lf KB" ), pmcProcMemCounter.QuotaPeakPagedPoolUsage/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );
    
        // Working set size
        csTempStr.Format( _T( "Working set size: %.02lf KB" ), pmcProcMemCounter.WorkingSetSize/1024.0f );
        m_ctvProcessDetails.InsertItemEx( csTempStr, nIconIndex, nIconIndex, hPInfoItem );

        m_ctvProcessDetails.Expand( hPInfoItem, TVE_EXPAND );
    }// End if
}// End LoadMemoryDetailsForProcess

void CProcessViewerDlg::LoadVersionInfoDetailsForProcess( PROC_INFO_t& stpiProcInfo_i )
{
    HTREEITEM hVersionItem = m_ctvProcessDetails.InsertItemEx( _T( "Version details" ), 21, 21 );
    const int nIconIndex = 63;
    if( hVersionItem )
    {
        // Get file version
        CString csVersionInfo;
        FileVersionInfo& fiFileInfo = stpiProcInfo_i.fviVersion;
        
        // Insert version information
        CString csInfo;
        if( fiFileInfo.GetCompanyName( csInfo ))
        {
            csVersionInfo.Format( _T( "Company: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }// End if

        if( fiFileInfo.GetFileVersion( csInfo ))
        {
            csVersionInfo.Format( _T( "File Version: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }
        
        if( fiFileInfo.GetProductVersion( csInfo ))
        {
            csVersionInfo.Format( _T( "Product Version: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }// End if

        if( fiFileInfo.GetFileDescription( csInfo ))
        {
            csVersionInfo.Format( _T( "File description: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }
        
        if( fiFileInfo.GetProductName( csInfo ))
        {
            csVersionInfo.Format( _T( "Product name: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }
        
        if( fiFileInfo.GetComments( csInfo ))
        {
            csVersionInfo.Format( _T( "Comments: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }

        if( fiFileInfo.GetLegalCopyright( csInfo ))
        {
            csVersionInfo.Format( _T( "Legal copyright: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }

        if( fiFileInfo.GetLegalTrademarks( csInfo ))
        {
            csVersionInfo.Format( _T( "Legal trademarks: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }

        if( fiFileInfo.GetPrivateBuild( csInfo ))
        {
            csVersionInfo.Format( _T( "Private build: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }

        if( fiFileInfo.GetSpecialBuild( csInfo ))
        {
            csVersionInfo.Format( _T( "Special build: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }

        if( fiFileInfo.GetOriginalFileName( csInfo ))
        {
            csVersionInfo.Format( _T( "Original filename: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }

        if( fiFileInfo.GetInternalName( csInfo ))
        {
            csVersionInfo.Format( _T( "Internal name: %s" ), csInfo );
            m_ctvProcessDetails.InsertItemEx( csVersionInfo, nIconIndex, nIconIndex, hVersionItem );
        }
        m_ctvProcessDetails.Expand( hVersionItem, TVE_EXPAND );
    }// End if
}// End LoadVersionInfoDetailsForProcess


/** 
 * 
 * Loads module information for given process.
 * 
 * @param       stpiProcInfo_i - Process information
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::LoadModuleInformationForProcess( PROC_INFO_t& stpiProcInfo_i )
{
    // Get module information list
    PMODULEINFO_LIST_t pstModuleInfo = stpiProcInfo_i.pstModuleInfoList;
    if( !pstModuleInfo )
    {
        return;
    }

    // Add process modules to redraw manager
    Utils::AutoSetRedrawManager asrmRedrMgr;
    asrmRedrMgr.AddWindow( m_clvProcessModules );

    // Clear previous entries
    m_clvProcessModules.DeleteAllItems();

	// Loop index
	int nIndex = 0;

    // Loop through and insert details into list
    for( nIndex = 0; pstModuleInfo; pstModuleInfo = pstModuleInfo->pstNextModuleInfo, ++nIndex )
    {
        int nSubItemIndex = 1;

        CString csIndex;
        csIndex.Format( _T( "%d" ), nIndex + 1 );
        m_clvProcessModules.InsertItem( nIndex, csIndex, 24 );

        // Set item data for item
        m_clvProcessModules.SetItemData( nIndex, RCAST( DWORD, pstModuleInfo ));

        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, pstModuleInfo->csModuleBaseName );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, pstModuleInfo->csModuleFullPath );

        CString csTempStr;
        csTempStr.Format( _T( "0x%p" ), pstModuleInfo->stmiModInfo.lpBaseOfDll );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csTempStr );

        csTempStr.Format( _T( "0x%X" ), pstModuleInfo->stmiModInfo.SizeOfImage );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csTempStr );

        csTempStr.Format( _T( "0x%p" ), pstModuleInfo->stmiModInfo.EntryPoint );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csTempStr );

        if( !pstModuleInfo->fviVersion.IsVersionRetrieved())
        {
            pstModuleInfo->fviVersion.GetInfo();
        }

        CString csModuleVersion;
        pstModuleInfo->fviVersion.GetProductVersion( csModuleVersion ); 
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csModuleVersion );

        CString csSize;
        Utils::GetFormattedSize( csSize, pstModuleInfo->fviVersion.GetFileSize() );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csSize );

        CString csCompany;
        pstModuleInfo->fviVersion.GetCompanyName( csCompany );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csCompany );
        
        CString csDescription;
        pstModuleInfo->fviVersion.GetFileDescription( csDescription );
        m_clvProcessModules.SetItemText( nIndex, nSubItemIndex++, csDescription );
    }// End for

    UpdateModuleCount( nIndex );
}// LoadModuleInformationForProcess

// LoadGDIInformation
void CProcessViewerDlg::LoadGDIInformation( PROC_INFO_t& stpiProcInfo_i )
{
    HTREEITEM hGUIResItem = m_ctvProcessDetails.InsertItemEx( _T( "GUI Resource usage" ), 41, 41 );
    
    // Get GDI object details
    stpiProcInfo_i.dwGDIObjs = GetGuiResources( stpiProcInfo_i.ahmProcessHandle, GR_GDIOBJECTS );
    stpiProcInfo_i.dwUserObjs = GetGuiResources( stpiProcInfo_i.ahmProcessHandle, GR_USEROBJECTS );

    CString csGUIStr;
    csGUIStr.Format( _T( "GDI objects: %lu" ), stpiProcInfo_i.dwGDIObjs );
    m_ctvProcessDetails.InsertItemEx( csGUIStr, 63, 63, hGUIResItem );

    csGUIStr.Format( _T( "User objects: %lu" ), stpiProcInfo_i.dwUserObjs );
    m_ctvProcessDetails.InsertItemEx( csGUIStr, 63, 63, hGUIResItem );

    // Expand
    m_ctvProcessDetails.Expand( hGUIResItem, TVE_EXPAND );
}

void CProcessViewerDlg::LoadProcessPrivileges( PROC_INFO_t& stpiProcInfo_i )
{
    Utils::AutoHandleMgr ahmToken;
    if( !OpenProcessToken( stpiProcInfo_i.ahmProcessHandle, 
                           TOKEN_READ,
                           &ahmToken.m_hHandle ))
    {
        TRACE_ERR( "Failed to open process for loading process privileges" );
        return;
    }

    DWORD dwBufferSize = 0;
    GetTokenInformation( ahmToken, TokenPrivileges, NULL, 0, &dwBufferSize );
    if( !dwBufferSize )
    {
        TRACE_ERR( "Failed to get token information buffer size" );
        return;
    }

    // Prepare buffer size
    PTOKEN_PRIVILEGES pTkPrivs = RCAST( PTOKEN_PRIVILEGES, new BYTE[dwBufferSize] );
    if( !pTkPrivs )
    {
        return;
    }

    // Auto delete
    Utils::AutoDeleter<TOKEN_PRIVILEGES, true> adPrivDeletor( &pTkPrivs );

    // Retrieve information
    if( !GetTokenInformation( ahmToken, TokenPrivileges, pTkPrivs, dwBufferSize, &dwBufferSize ))
    {
        return;
    }

    // Privilege item
    HTREEITEM hPrivItem = m_ctvProcessDetails.InsertItemEx( _T( "Process privileges" ), 29, 29 );

    // Privilege name buffer
    TCHAR szPrivName[MAX_PATH] = { 0 };
    for( DWORD dwIndex = 0; dwIndex < pTkPrivs->PrivilegeCount; ++dwIndex )
    {
        if( !Utils::IsValidMask( pTkPrivs->Privileges[dwIndex].Attributes, SE_PRIVILEGE_ENABLED ) &&
            !Utils::IsValidMask( pTkPrivs->Privileges[dwIndex].Attributes, SE_PRIVILEGE_ENABLED_BY_DEFAULT ) &&
            !Utils::IsValidMask( pTkPrivs->Privileges[dwIndex].Attributes, SE_PRIVILEGE_USED_FOR_ACCESS ))
        {
            continue;
        }

        DWORD dwNameLength = MAX_PATH;
        if( !LookupPrivilegeName( 0, &pTkPrivs->Privileges[dwIndex].Luid, szPrivName, &dwNameLength ))
        {
            continue;
        }

        // Store name
        CString csPrivName = szPrivName;

        // Reset length
        dwNameLength  = MAX_PATH;

        // Language id
        DWORD dwLangId = 0;

        // Get detailed description of privilege
        if( LookupPrivilegeDisplayName( 0, csPrivName, szPrivName, &dwNameLength, &dwLangId ))
        {
            csPrivName += _T( " - " );
            csPrivName += szPrivName;
        }

        m_ctvProcessDetails.InsertItemEx( csPrivName, 63, 63, hPrivItem );
    }// End for

    // Expand
    m_ctvProcessDetails.Expand( hPrivItem, TVE_EXPAND );
}// End LoadProcessPrivileges

void CProcessViewerDlg::OnSelchangedTreeProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW* pNMTreeView = ( NM_TREEVIEW* )pNMHDR;

    // Get node data
    PINodeData pNodeData = m_ctvProcess.GetItemData( pNMTreeView->itemNew.hItem );
    if( !pNodeData || pNodeData->GetItemType() != IT_PROC_INFO )
    {
        return;
    }

    // Cast out item data
    PPROC_INFO_t pstProcInfo = DCAST( PPROC_INFO_t, pNodeData );
    if( !pstProcInfo->fviVersion.IsVersionRetrieved())
    {
        pstProcInfo->fviVersion.GetInfo();
    }


    BeginPerformance();

    // If initialized
    if( !pstProcInfo->pscSymbolCollection.IsInitialized() )
    {
        UpdateProgressText( _T( "Please wait... Gathering symbols" ));
        if( pstProcInfo->pscSymbolCollection.Init( pstProcInfo->ahmProcessHandle, 
                                                   pstProcInfo->csFullPath ))
        {
            UpdateProgressText( _T( "Successfully loaded symbols" ));
        }
        else
        {
            UpdateProgressText( _T( "Failed to load symbols!" ));
        }// End if

        // Trigger timer for setting default message
        UpdateProgressText( 0 );
    }// End if

    if( pstProcInfo->pscSymbolCollection.IsInitialized() )
    {
        LoadModuleSymbolInformation( pstProcInfo->pscSymbolCollection, 
                                     RCAST( DWORD, pstProcInfo->stmiModInfo.lpBaseOfDll ),
                                     0,
                                     0 );
    }

    Utils::AutoSetRedrawManager asrmRedrMgr; 
    asrmRedrMgr.AddWindow( m_ctvProcessDetails );

    // Delete previous information
    m_ctvProcessDetails.DeleteAllItems();

    // Insert process owner
    CString csOwner;
    if( Process::ExtractProcessOwner( pstProcInfo->ahmProcessHandle, csOwner ))
    {
        CString csText;
        csText.Format( _T( "Owner: %s" ), SCAST( LPCTSTR, csOwner ));
        m_ctvProcessDetails.InsertItemEx( csText, 5, 5 );
    }

    CString csCommandLine;
    if( Process::ExtractProcessCommandLine( pstProcInfo->ahmProcessHandle, csCommandLine ) &&
        !csCommandLine.IsEmpty() )
    {
        CString csText;
        csText.Format( _T( "Cmd line: %s" ), SCAST( LPCTSTR, csCommandLine ));
        m_ctvProcessDetails.InsertItemEx( csText, 30, 30 );
    }

    if( g_Settings.m_bProcessType )
    {
        // Insert application type
        m_ctvProcessDetails.InsertItemEx( pstProcInfo->csApplicationType, 32, 32 );
    }

    if( m_AutoStartupInfo.Lookup( pstProcInfo->csFullPath ))
    {
      m_ctvProcessDetails.InsertItemEx( _T( "Auto startup" ), 47, 47 );
    }

    if( g_Settings.m_bProcessPriority )
    {
        // Process priority
        GetProcessPriority( pstProcInfo->ahmProcessHandle, pstProcInfo->csPriority, pstProcInfo->uProcessPriority );

        // Insert process priority
        m_ctvProcessDetails.InsertItemEx( pstProcInfo->csPriority, 12, 12 );
    }

    if( g_Settings.m_bProcessGDIResInfo )
    {
        // Load GUI information
        LoadGDIInformation( *pstProcInfo );
    }

    if( g_Settings.m_bProcessSize )
    {
        // Insert size
        CString csSize;
        Utils::GetFormattedSize( csSize, pstProcInfo->fviVersion.GetFileSize() );
        CString csSizeFormat;
        csSizeFormat.Format( _T( "Size: %s" ), csSize );
        m_ctvProcessDetails.InsertItemEx( csSizeFormat, 43, 43 );
    }

    if( g_Settings.m_bProcessHwnd )
    {
        // Load window details for a process
        LoadWindowDetailsForProcess( *pstProcInfo );
    }

    if( g_Settings.m_bProcessTimes )
    {
        // Load process times for a process
        LoadProcessTimesForProcess( *pstProcInfo );
    }

    if( g_Settings.m_bProcessFileTimes )
    {
        // Load file times for a process
        LoadFileTimesForProcess( *pstProcInfo );
    }

    if( g_Settings.m_bProcessIOCounters )
    {
        LoadIOCounters( *pstProcInfo );
    }

    if( g_Settings.m_bProcessMemInfo )
    {
        // Load memory details for process
        LoadMemoryDetailsForProcess( *pstProcInfo );
    }

    if( g_Settings.m_bProcessVersion )
    {
        // Load version information for process
        LoadVersionInfoDetailsForProcess( *pstProcInfo );
    }

    if( g_Settings.m_bProcessPrivileges )
    {
        // Load privilege information for process
        LoadProcessPrivileges( *pstProcInfo );
    }

    // Load module information for process
    LoadModuleInformationForProcess( *pstProcInfo );
    *pResult = 0;

    EndPerformance();
}// End OnSelchangedTreeProcess

void CProcessViewerDlg::UpdateModuleCount( const int nCount_i )
{
    CString csText;
    csText.Format( ID_SP_MODULE_COUNT, nCount_i );
    m_StatusBar.SetPaneText( 2, csText, TRUE );
}

void CProcessViewerDlg::UpdateProcessCount( const int nCount_i )
{
    CString csText;
    csText.Format( ID_SP_PROCESS_COUNT, nCount_i );
    m_StatusBar.SetPaneText( 1, csText, TRUE );
}

void CProcessViewerDlg::UpdateCount()
{
    UpdateModuleCount( 0 );
    UpdateProcessCount( 0 );
}

void CProcessViewerDlg::BeginPerformance()
{
    // Get pane index for given command
    const int nPaneIndex = m_StatusBar.CommandToIndex( ID_SP_PERFORMANCE );

    // Get performance
    CString csText;
    csText.Format( _T( "..." ));

    // Set pane text
    m_StatusBar.SetPaneText( nPaneIndex, csText, TRUE );

    // Start performance
    m_Perf.Start();
}

void CProcessViewerDlg::EndPerformance()
{
    // Get performance
    const double dblPerf = m_Perf.Stop();

    // Get pane index for given command
    const int nPaneIndex = m_StatusBar.CommandToIndex( ID_SP_PERFORMANCE );

    // Text
    CString csText;
    csText.Format( _T( "Time taken - %lf sec" ), dblPerf );

    // Set pane text
    m_StatusBar.SetPaneText( nPaneIndex, csText, TRUE );
}


BOOL CProcessViewerDlg::PreTranslateMessage( MSG* pMsg )
{
    if( m_hAccel && TranslateAccelerator( this->GetSafeHwnd(), m_hAccel, pMsg ))
    {
        return TRUE;
    }
    else
        if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
        {
            return TRUE;
        }
    
    return CDialog::PreTranslateMessage( pMsg );
}

void CProcessViewerDlg::OnOptionsRefresh() 
{
    SetTimer( TIMER_REFRESH_PROCESS_INFORMATION, 0, 0 );
}

void CProcessViewerDlg::OnOptionsSwaplayout() 
{
    if( m_MainDivider.GetSizingBar().IsVertical() )
    {
        m_MainDivider.SetSizingBarDirection( DIR_HORZ );
    }
    else
    {
        m_MainDivider.SetSizingBarDirection( DIR_VERT );
    }
}

void CProcessViewerDlg::OnSize( UINT uType_i, int nCx_i, int nCy_i )
{
    // Get client rect
    CRect crClientRect;
    GetClientRect( &crClientRect );

    // Will hold new client area co-ordinates
    CRect crNewClientArea;

    // Query how much space will the status bars take
    RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, &crNewClientArea );

    // New client area
    crNewClientArea.DeflateRect( 2, 2 );

    // Reposition divider
    m_MainDivider.GetSafeHwnd() && m_MainDivider.ResizeToFitParent( &crNewClientArea );

    // Now move status bars and tool bars to it's actual position
    RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );
    SetUpProgressBar();

    CDialog::OnSize( uType_i, nCx_i, nCy_i );
}// End OnSize

void CProcessViewerDlg::OnOptionsExit() 
{
    CDialog::EndDialog( 0 );
}

void CProcessViewerDlg::OnOptionsAbout() 
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void CProcessViewerDlg::OnOptionsShowpath() 
{
    const BOOL bFullPathChecked = m_ToolBar.GetToolBarCtrl().IsButtonChecked( ID_OPTIONS_SHOWPATH );
    HTREEITEM hItem = m_ctvProcess.GetNextItem( m_ctvProcess.GetRootItem(), TVGN_CHILD );
    while( hItem )
    {
        PINodeData pNodeData = m_ctvProcess.GetItemData( hItem );
        if( !pNodeData || pNodeData->GetItemType() != IT_PROC_INFO )
        {
            return;
        }

        // Get proc info
        PPROC_INFO_t pstProcInfo = DCAST( PPROC_INFO_t, pNodeData );
        
        CString csProc;
        csProc.Format( _T( "%s - %lu" ), SCAST( LPCTSTR , ( bFullPathChecked ? pstProcInfo->csFullPath : pstProcInfo->csBaseName )), pstProcInfo->dwProcId );
        m_ctvProcess.SetItemText( hItem, csProc );

        // Get next sibling
        hItem = m_ctvProcess.GetNextItem( hItem, TVGN_NEXT );
    }// End while
}// End OnOptionsShowpath


void CProcessViewerDlg::OnDblclkListProcessdetails(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // If toolbar button not checked then return
    if( !m_ToolBar.GetToolBarCtrl().IsButtonChecked( ID_OPTIONS_ENABLEDEPENDS ))
    {
        return;
    }

    LPNMLISTVIEW pListNM = RCAST( LPNMLISTVIEW, pNMHDR );   
    *pResult = 0;

    TCHAR szPath[MAX_PATH] = { 0 };
    m_clvProcessModules.GetItemText( pListNM->iItem, 2, szPath, MAX_PATH );
    
    if( szPath[0] )
    {
        CString csName;
        csName.Format( _T( "\"%s\"" ), szPath );
        ShellExecute( this->GetSafeHwnd(), 0, _T( "depends.exe" ), csName, 0, SW_SHOWDEFAULT );
    }
}

void CProcessViewerDlg::OnDblclkTreeProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // If toolbar button is not checked then return
    if( !m_ToolBar.GetToolBarCtrl().IsButtonChecked( ID_OPTIONS_ENABLEDEPENDS ))
    {
        return;
    }

    UNREFERENCED_PARAMETER( pNMHDR );
    *pResult = 0;

    HTREEITEM hSelectedItem = m_ctvProcess.GetSelectedItem();
    if( m_ctvProcess.GetNextItem( hSelectedItem, TVGN_PARENT ) == m_ctvProcess.GetRootItem())
    {
        // Node data information
        PINodeData pNodeData = m_ctvProcess.GetItemData( hSelectedItem );
        if( !pNodeData || pNodeData->GetItemType() != IT_PROC_INFO )
        {
            return;
        }

        // Procedure information
        PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );

        CString csName;
        csName.Format( _T( "\"%s\"" ), pProcInfo->csFullPath );
        ShellExecute( this->GetSafeHwnd(), 0, m_csDependsPath, csName, 0, SW_SHOWDEFAULT );
    }// End if
}// OnDblclkTreeProcess

void CProcessViewerDlg::OnRClkTreeProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
    UNREFERENCED_PARAMETER( pNMHDR );
    *pResult = 0;

    PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
    if( !pNodeData || pNodeData->GetItemType() != IT_PROC_INFO || m_ctvProcess.GetSelectedItem() != m_ctvProcess.GetItemUnderCursor() )
    {
        return;
    }

    PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
    if( !pProcInfo )
    {
        return;
    }

    // Menu
    CMenu cmMenu;
    VERIFY( cmMenu.LoadMenu( IDR_MENU_PROCESS_UTILS ));

    CMenu cmSubMenu;
    cmSubMenu.Attach( cmMenu.GetSubMenu( 0 )->GetSafeHmenu() );
    switch( pProcInfo->uProcessPriority )
    {
    case REALTIME_PRIORITY_CLASS:
        cmSubMenu.CheckMenuItem( ID_PRIORITY_REALTIME, MF_CHECKED | MF_BYCOMMAND );
        break;
    case HIGH_PRIORITY_CLASS:
        cmSubMenu.CheckMenuItem( ID_PRIORITY_HIGH, MF_CHECKED | MF_BYCOMMAND );
        break;
    case ABOVE_NORMAL_PRIORITY_CLASS:
        cmSubMenu.CheckMenuItem( ID_PRIORITY_ABOVENORMAL, MF_CHECKED | MF_BYCOMMAND );
        break;
    case NORMAL_PRIORITY_CLASS:
        cmSubMenu.CheckMenuItem( ID_PRIORITY_NORMAL, MF_CHECKED | MF_BYCOMMAND );
        break;
    case BELOW_NORMAL_PRIORITY_CLASS:
        cmSubMenu.CheckMenuItem( ID_PRIORITY_BELOWNORMAL, MF_CHECKED | MF_BYCOMMAND );
        break;
    case IDLE_PRIORITY_CLASS:
        cmSubMenu.CheckMenuItem( ID_PRIORITY_LOW, MF_CHECKED | MF_BYCOMMAND );
        break;
    }

    VERIFY( GetProcessPriorityBoost( pProcInfo->ahmProcessHandle, &( pProcInfo->bPriorityBoost )));
    const UINT uCheckStatus = pProcInfo->bPriorityBoost ? MF_CHECKED : MF_UNCHECKED;
    cmSubMenu.CheckMenuItem( ID_PRIORITY_DISABLEBOOSTPRIORITY, uCheckStatus | MF_BYCOMMAND );

    // Current message, for positioning menu
    const MSG* msgCurMsg = GetCurrentMessage();

    // Show popup menu
    cmSubMenu.TrackPopupMenu( TPM_LEFTALIGN, msgCurMsg->pt.x, msgCurMsg->pt.y, this );
}// OnRClkTreeProcess

void CProcessViewerDlg::OnOptionsEnabledepends() 
{
    // Search only once
    if( !m_bSearchedInPath )
    {
        if( !Utils::FindFile( _T( "depends.exe" ), m_csDependsPath ))
        {
            m_ToolBar.GetToolBarCtrl().CheckButton( ID_OPTIONS_ENABLEDEPENDS, FALSE );
            m_ToolBar.GetToolBarCtrl().EnableButton( ID_OPTIONS_ENABLEDEPENDS, FALSE );
            MessageBox( _T( "Dependency walker not found in path!" ), _T( "Error!" ), MB_OK | MB_ICONERROR );
        }
        else
        {
           m_bSearchedInPath = true;
        }// End if
    }// End if
}// End OnOptionsEnabledepends

void CProcessViewerDlg::OnOptionsSearch() 
{
    SearchDlg srchDlg;
    if( srchDlg.DoModal() != IDOK )
    {
        return;
    }

    // If search string is empty then do a full search
    if( g_Settings.m_csLastSrchString.IsEmpty() )
    {
        RefreshProcessInformation( 0 );
        return;
    }

    // Fill out search criteria
    SEARCH_CRITERIA srchCriteria;
    srchCriteria.eSrchType = g_Settings.m_nLastSearchChoice == 0 ? ST_DLL : ST_PROCESS;
    srchCriteria.csSrch = g_Settings.m_csLastSrchString;

    // Get details of process
    RefreshProcessInformation( &srchCriteria );
}// End if


/** 
 * 
 * Modifies environment PATH variable to include drivers directory. Helps
 * SearchPath to find out drivers too if drivers path is included in PATH var.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::ModifyEnvPATH() const
{
    // Modify path variable to point to drivers directory too
    // since when using SearchPath it can search this directory too for this
    // driver files
    const int PATH_BUFF_SIZE = 20*1024;
    TCHAR szPath[ PATH_BUFF_SIZE ] = { 0 };
    GetEnvironmentVariable( _T( "path" ), szPath, PATH_BUFF_SIZE );

    TCHAR szSystemRoot[MAX_PATH];
    GetEnvironmentVariable( _T( "systemroot" ), szSystemRoot, MAX_PATH );

    CString csName;
    csName.Format( _T( ";%s\\System32\\Drivers\\" ), szSystemRoot );

    // Modify path
    _tcscat_s( szPath, PATH_BUFF_SIZE, csName );
    SetEnvironmentVariable( _T( "path" ), szPath );
}


/** 
 * 
 * Prepare access. Enables debug privilege for process.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::SetUpAccess() const
{
    Utils::AutoHandleMgr ahmProcToken; 
    if( !OpenProcessToken( GetCurrentProcess(), 
                           TOKEN_ALL_ACCESS,
                           &ahmProcToken.m_hHandle ))
    {
        return;
    }

    // List of privileges, all of them, he he ;)
    LPCTSTR lpctszPrivileges[] = { SE_CREATE_TOKEN_NAME,
                                   SE_ASSIGNPRIMARYTOKEN_NAME,
                                   SE_LOCK_MEMORY_NAME,
                                   SE_INCREASE_QUOTA_NAME,
                                   SE_UNSOLICITED_INPUT_NAME,
                                   SE_MACHINE_ACCOUNT_NAME,
                                   SE_TCB_NAME,
                                   SE_SECURITY_NAME,
                                   SE_TAKE_OWNERSHIP_NAME,
                                   SE_LOAD_DRIVER_NAME,
                                   SE_SYSTEM_PROFILE_NAME,
                                   SE_SYSTEMTIME_NAME,
                                   SE_PROF_SINGLE_PROCESS_NAME,
                                   SE_INC_BASE_PRIORITY_NAME,
                                   SE_CREATE_PAGEFILE_NAME,
                                   SE_CREATE_PERMANENT_NAME,
                                   SE_BACKUP_NAME,
                                   SE_RESTORE_NAME,
                                   SE_SHUTDOWN_NAME,
                                   SE_DEBUG_NAME,
                                   SE_AUDIT_NAME,
                                   SE_SYSTEM_ENVIRONMENT_NAME,
                                   SE_CHANGE_NOTIFY_NAME,
                                   SE_REMOTE_SHUTDOWN_NAME,
                                   SE_UNDOCK_NAME,
                                   SE_SYNC_AGENT_NAME,          
                                   SE_ENABLE_DELEGATION_NAME,   
                                   SE_MANAGE_VOLUME_NAME,
                                   SE_IMPERSONATE_NAME,
                                   SE_CREATE_GLOBAL_NAME,
                                   0 };

    TOKEN_PRIVILEGES tkpPriv = { 0 };
    for( int nIndex = 0; lpctszPrivileges[nIndex]; ++nIndex )
    {
        ::ZeroMemory( &tkpPriv, sizeof( tkpPriv ));
        LookupPrivilegeValue( NULL, 
                              lpctszPrivileges[nIndex], 
                              &tkpPriv.Privileges[0].Luid );
        tkpPriv.PrivilegeCount = 1;
        tkpPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges( ahmProcToken, FALSE, &tkpPriv, 0, 0, 0 );
    }// End for
}// End SetUpAccess


/** 
 * 
 * Timer that helps in asynchronously triggering a load process. On first
 * call itself timer will be killed. Since our purpose of triggering an asynchronous call
 * is done. 
 * 
 * @param       nIDEvent - Timer id determines type of asynchronous request.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::OnTimer(UINT nIDEvent) 
{
    CDialog::OnTimer(nIDEvent);
    
    // Timer not needed again
    KillTimer( nIDEvent );

    switch( nIDEvent )
    {
    case TIMER_REFRESH_PROCESS_INFORMATION:
        {
            RefreshProcessInformation( 0 );
            break;
        }
    case TIMER_SET_STATUS_TEXT_DEDICATION:
        {
            // Get pane index of dedication message
            const int nDedPaneIndex = m_StatusBar.CommandToIndex( ID_SP_COPYRIGHT );

            CString csDedication;
            csDedication.LoadString( ID_SP_COPYRIGHT );
            m_StatusBar.SetPaneText( nDedPaneIndex, csDedication );
			break;
        }
	case TIMER_LOAD_AUTOSTARUP_INFO:
		{
			VERIFY( m_AutoStartupInfo.Load());
			break;
		}
    }// End switch
}


/** 
 * 
 * Update progress message. If progress message is null then default copyright message is set.
 * 
 * @param       lpctszProgressText_i - Progress message
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::UpdateProgressText( LPCTSTR lpctszProgressText_i )
{
    const int nDedicationPaneIndex = m_StatusBar.CommandToIndex( ID_SP_COPYRIGHT );
    if( lpctszProgressText_i )
    {
        m_StatusBar.SetPaneText( nDedicationPaneIndex, lpctszProgressText_i );
    }
    else
    {
        SetTimer( TIMER_SET_STATUS_TEXT_DEDICATION, WAIT_TIME_FOR_SETTING_DEFAULT_TEXT, 0 );
    }// End if
}



/** 
 * 
 * Updates progress bar progress position.
 * 
 * @param       nProgress_i - Progress
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::UpdateProgressBar( const int nProgress_i )
{
    m_PrgCtrl.SetPos( nProgress_i );
}



/** 
 * 
 * Set's up a progress bar. Helps in positioning progress right on the statusbar.
 * Called also while repositioning progress bar
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::SetUpProgressBar()
{
    if( !m_StatusBar.GetSafeHwnd() || !m_PrgCtrl.GetSafeHwnd() )
    {
        return;
    }

    CRect crPaneRect;
    m_PrgCtrl.SetParent( &m_StatusBar );
    m_StatusBar.GetStatusBarCtrl().GetRect( 1, &crPaneRect );

    // Adjust rectangle
    crPaneRect.DeflateRect( 0, 2, 2, 2 );

    // Move progress rectangle into view
    m_PrgCtrl.MoveWindow( crPaneRect );
}


/** 
 * 
 * Refreshes start and end point for a progress bar.
 * 
 * @param       nMax_i        - Max progress limit
 * @param       nCurrentPos_i - Current position of progress bar
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::ReInitProgressBar( const int nMax_i, int nCurrentPos_i )
{
    m_PrgCtrl.SetRange32( 0, nMax_i );
    m_PrgCtrl.SetPos( nCurrentPos_i );
}


/** 
 * 
 * Returns file path from a file string. For eg: \\??\\C:\SomeBlah.exe.
 * Since there is a path hidden we can extract it.
 * 
 * @param       csFileName_io - 
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CProcessViewerDlg::ParseOutFileName( CString& csFileName_io ) const
{
    // If there is a colon in this path then there should be a path hidden inside it
    const int nIndex = csFileName_io.Find( _T( ':' ));
    if( nIndex == -1 )
    {
        // return
        return;
    }

    // Copy valid path
    csFileName_io = csFileName_io.Mid( nIndex - 1 );
}



/** 
 * 
 * Returns associated file icon. For eg: for winword exe this function
 * returns the winword icon.
 * 
 * @param       lpctszFilePath_i - File path
 * @return      HICON            - Return icon for file
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
HICON CProcessViewerDlg::GetAssociatedFileIcon( LPCTSTR lpctszFilePath_i )
{
    SHFILEINFO shFileInfo = { 0 };

    // For retrieving icon
    VERIFY( SHGetFileInfo( lpctszFilePath_i, 
                           FILE_ATTRIBUTE_NORMAL, 
                           &shFileInfo, 
                           sizeof( shFileInfo ), 
                           SHGFI_SMALLICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES ));

    // Do we have an icon, then store this icon handle
    // for destruction later on
    if( shFileInfo.hIcon )
    {
        // Add icon to list for destroying later on
        m_lstExtraIcons.AddTail( shFileInfo.hIcon );
    }

    // Icon to return
    return shFileInfo.hIcon;
}


/** 
 * 
 * Returns type of exe file. i.e. whether it's a console application or a
 * windows application or an MS-DOS application
 * 
 * @param       lpctszFilePath_i - File to check
 * @param       csFileType_o     - On return will hold file type
 * @return      bool             - Return execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool CProcessViewerDlg::GetExeType( LPCTSTR lpctszFilePath_i, CString& csFileType_o )
{
   SHFILEINFO shFileInfo = { 0 };
    // Second call is for retrieving file type
    const DWORD dwRetVal = SHGetFileInfo( lpctszFilePath_i, 
                                          FILE_ATTRIBUTE_NORMAL, 
                                          &shFileInfo, 
                                          sizeof( shFileInfo ), 
                                          SHGFI_EXETYPE );
    if( dwRetVal )
    {
       /************************************************************************
           dwRetVal is interpreted as follows...
           LOWORD = NE or PE and HIWORD = 3.0, 3.5, or 4.0  Windows application 
           LOWORD = MZ and HIWORD = 0  MS-DOS .exe, .com, or .bat file 
           LOWORD = PE and HIWORD = 0  Win32 console application 
        ************************************************************************/

        const WORD wLowWord =  LOWORD( dwRetVal );
        const WORD wHiWord = HIWORD( dwRetVal );
        const WORD wPEWord = MAKEWORD( 'P', 'E' );
        const WORD wMZWord = MAKEWORD( 'M', 'Z' );
        const WORD wNEWord = MAKEWORD( 'N', 'E' );

        // Read above comments to understand what's happening
        if( wLowWord == wPEWord || wLowWord == wNEWord )
        {
            if( wHiWord == 0 )
            {
                csFileType_o = _T( "Win32 Console Application" );
            }
            else
            {
                csFileType_o = _T( "Windows application" );
            }// End if
        }
        else if( wLowWord == wMZWord && wHiWord == 0 )
        {
            csFileType_o = _T( "MS-DOS .exe, .com or .bat file" );
        }
        else
        {
            csFileType_o = _T( "Unknown file type" );
            return false;
        }// End if
    }// End if

    return true;
}// End GetExeType


void CProcessViewerDlg::OnOptionsSettings() 
{
    OptionsDlg opt;
    opt.DoModal();
}

void CProcessViewerDlg::OnOptionsKillprocess() 
{
    PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
    if( !pNodeData || 
        pNodeData->GetItemType() != IT_PROC_INFO )
    {
        return;
    }

    PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
    if( g_Settings.m_bPromptBeforeKillingProcess && 
        Utils::ShowQuestion( _T( "Are you sure that you want to kill '%s'" ), pProcInfo->csBaseName ) != IDYES )
    {
        return;
    }

    if( !TerminateProcess( pProcInfo->ahmProcessHandle, 12 ))
    {
        Utils::ShowError( _T( "Failed to kill process" ));
    }
    else // Termination of process succeeded
    {
        // Update progress text
        UpdateProgressText( _T( "Killed process, snapshot retained" ));
        pProcInfo->bProcessKilled = true;

        // Change display of item
        m_ctvProcess.SetItemState( m_ctvProcess.GetSelectedItem(), TVIS_BOLD, TVIS_BOLD );

        // Set default text
        UpdateProgressText( 0 );
    }// End if
}// End OnOptionsKillprocess

void CProcessViewerDlg::OnOptionsKillprocessAllInstances()
{
   PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
   if( !pNodeData || 
       pNodeData->GetItemType() != IT_PROC_INFO )
   {
      return;
   }
   
   PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
   if( g_Settings.m_bPromptBeforeKillingProcess && 
       Utils::ShowQuestion( _T( "All instances of the process '%s' will be killed, are you sure?" ), pProcInfo->csBaseName ) != IDYES )
   {
      return;
   }

   // Backup full path of selected process, so that we can kill
   // all of them, Mwhahahahahaha
   CString csProcessFullPath = pProcInfo->csFullPath;

   HTREEITEM hRootItem = m_ctvProcess.GetRootItem();
   HTREEITEM hNextItem = m_ctvProcess.GetNextItem( hRootItem, TVGN_CHILD );
   int nCountOfKilledProcesses = 0;
   while( hNextItem )
   {
      PINodeData pNodeData = m_ctvProcess.GetItemData( hNextItem );
      if( pNodeData )
      {
         PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
         if( pProcInfo )
         {
            // Path should match and 
            if( pProcInfo->csFullPath == csProcessFullPath && !pProcInfo->bProcessKilled )
            {
               if( !TerminateProcess( pProcInfo->ahmProcessHandle, 12 ))
               {
                  g_Log.Write( _T( "Failed to kill process %s, pid: %lu" ), pProcInfo->csFullPath, pProcInfo->dwProcId );
               }
               else
               {
                  // Update progress text
                  pProcInfo->bProcessKilled = true;

                  // Change display of item
                  m_ctvProcess.SetItemState( hNextItem, TVIS_BOLD, TVIS_BOLD );
                  ++nCountOfKilledProcesses;
               }// End if
            }// End if
         }// End if
      }// End if
      hNextItem = m_ctvProcess.GetNextItem( hNextItem, TVGN_NEXT );
   }// End while

   if( nCountOfKilledProcesses )
   {
      // Set default text
      CString csPrg;
      csPrg.Format( _T( "%d %s killed" ), nCountOfKilledProcesses, ( nCountOfKilledProcesses > 1 ? _T( "processes" ) : _T( "process" )));
      UpdateProgressText( csPrg );
   }

   // Set default text back
   UpdateProgressText( 0 );

}// OnOptionsKillprocessAllInstances


void CProcessViewerDlg::HandleModuleListSelectionChanged( const int nItem_i )
{
    if( nItem_i < 0 )
    {
        return;
    }

    // Process information
    PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
    if( !pNodeData || pNodeData->GetItemType() != IT_PROC_INFO )
    {
        // Skip since we don't want this information
        return;
    }

    // Procedure information
    PPROC_INFO_t pstProcInfo = RCAST( PPROC_INFO_t, pNodeData );

    // Get item data from list
    PMODULEINFO_LIST_t pModuleInfo = RCAST( PMODULEINFO_LIST_t,  
                                            m_clvProcessModules.GetItemData( nItem_i ));


    // Load module symbol information
    LoadModuleSymbolInformation( pstProcInfo->pscSymbolCollection,
                                 RCAST( DWORD, pModuleInfo->stmiModInfo.lpBaseOfDll ),
                                 pModuleInfo->csModuleFullPath,
                                 pModuleInfo->stmiModInfo.SizeOfImage );
}


bool CProcessViewerDlg::LoadModuleSymbolInformation( ProcessSymbolCollection& pscSymColl_i,
                                                     const DWORD dwBaseOfDll_i,
                                                     LPCTSTR lpctszModuleFileName_i,
                                                     const DWORD dwModuleSize_i )
{
    // Manages set redraw
    Utils::AutoSetRedrawManager asrmSetRedrawManager;
    asrmSetRedrawManager.AddWindow( m_clvModuleSymbolList.GetSafeHwnd() );

    // Remove previous entries from symbol list
    m_clvModuleSymbolList.DeleteAllItems();

    // Check if symbol collection has been initialized or not
    if( !pscSymColl_i.IsInitialized() )
    {
        return false;
    }

    SymbolCollection* pmscModSymColl = 0;
    pscSymColl_i.EnumerateModuleSymbols( dwBaseOfDll_i, pmscModSymColl, lpctszModuleFileName_i, dwModuleSize_i, false );

    if( pmscModSymColl && pmscModSymColl->StartTraversal())
    {
        PSymbolData pSymbData = 0;
        int nIndex = 0;
        while( pmscModSymColl->GetNext( pSymbData ))
        {
            CString csTemp;
            csTemp.Format( _T( "%d" ), nIndex + 1 );
            m_clvModuleSymbolList.InsertItem( nIndex, csTemp, 33 );

			// Get symbol name
			csTemp = pSymbData->m_SymbolInfo->Name;
            m_clvModuleSymbolList.SetItemText( nIndex, 1,  csTemp );

            csTemp.Format( _T( "%#04x" ), pSymbData->m_SymbolInfo->Address );
            m_clvModuleSymbolList.SetItemText( nIndex, 2, csTemp );

            csTemp.Format( _T( "%lu" ), pSymbData->m_SymbolInfo->Size );
            m_clvModuleSymbolList.SetItemText( nIndex, 3, csTemp );

			csTemp.Empty();
			pSymbData->GetSymbolType( csTemp );
			m_clvModuleSymbolList.SetItemText( nIndex, 4, csTemp );

            ++nIndex;
        }// End while
    }// End if

    return true;
}

void CProcessViewerDlg::OnItemchangedListProcessmodules(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if( m_clvProcessModules.GetItemState( pNMListView->iItem, LVIS_SELECTED ) == LVIS_SELECTED )
    {
        HandleModuleListSelectionChanged( pNMListView->iItem );
    }
    *pResult = 0;
}

void CProcessViewerDlg::OnGetInfoTipTreeProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVGETINFOTIP* pNMTVGetInfoTip = (NMTVGETINFOTIP*)pNMHDR;
	*pResult = 0;
    
    // Information tip
    if( !pNMTVGetInfoTip )
    {
        return;
    }

    PINodeData pNodeData = RCAST( PINodeData, pNMTVGetInfoTip->lParam );
    if( !pNodeData || pNodeData->GetItemType() != IT_PROC_INFO)
    {
        return;
    }

    PPROC_INFO_t pProcInfo = RCAST( PPROC_INFO_t, pNodeData );
    if( !pProcInfo  )
    {
        return;
    }

    // Retrieve version if not retrieved already
    if( !pProcInfo->fviVersion.IsVersionRetrieved() )
    {
        pProcInfo->fviVersion.GetInfo();
    }

    // Get process description
    CString csFileDescription;
    pProcInfo->fviVersion.GetFileDescription( csFileDescription );

    // Get company description
    CString csCompany;
    pProcInfo->fviVersion.GetCompanyName( csCompany );

    // Copy to buffer
    lstrcpyn( pNMTVGetInfoTip->pszText, 
              csFileDescription + _T( " - " ) + csCompany, 
              pNMTVGetInfoTip->cchTextMax );
}

void CProcessViewerDlg::HandleProcessPriorityCommand( const UINT uCmdId_i )
{
    PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
    if( !pNodeData )
    {
        ASSERT( FALSE );
        return;
    }

    if( pNodeData->GetItemType() == IT_PROC_INFO )
    {
        // Priority array
        const UINT uPriority[ID_PRIORITY_LOW-ID_PRIORITY_REALTIME+1] = { REALTIME_PRIORITY_CLASS, 
                                                                         HIGH_PRIORITY_CLASS, 
                                                                         ABOVE_NORMAL_PRIORITY_CLASS,
                                                                         NORMAL_PRIORITY_CLASS,
                                                                         BELOW_NORMAL_PRIORITY_CLASS,
                                                                         IDLE_PRIORITY_CLASS };

        // Selected priority index will be difference between realtime and the current command id
        const UINT uSelectedPriority = uPriority[uCmdId_i-ID_PRIORITY_REALTIME];

        PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
        ASSERT( pProcInfo );

        CString csText;
        if( !Process::ChangeProcessPriority( pProcInfo->ahmProcessHandle, uSelectedPriority ))
        {
            Utils::GetLastErrorMsg( 0 );
            csText.Format( _T( "Failed to change priority " ));
            UpdateProgressText( csText );
        }
        else
        {
            csText.Format( _T( "Priority changed" ));
            UpdateProgressText( csText );

            // Update stored priority
            pProcInfo->uProcessPriority = uSelectedPriority;
        }

        UpdateProgressText( 0 ); // Reset progress text to copyright message

    }// End if
}// End HandleProcessPriorityCommand

void CProcessViewerDlg::HandleBoostPriorityCommand()
{
    PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
    if( !pNodeData )
    {
        ASSERT( FALSE );
        return;
    }

    if( pNodeData->GetItemType() == IT_PROC_INFO )
    {
        PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );

        // Progress text
        CString csText;
        if( SetProcessPriorityBoost( pProcInfo->ahmProcessHandle, !pProcInfo->bPriorityBoost ))
        {
           csText.Format( _T( "Priority boost status changed" ));
           UpdateProgressText( csText );
        }
        else
        {
           csText.Format( _T( "Failed to change priority boost status " ));
           UpdateProgressText( csText );
        }// End if

        UpdateProgressText( 0 ); // Reset progress text to copyright message
    }// End if
}// End HandleBoostPriorityCommand

void CProcessViewerDlg::HandleOpenParentFolder()
{
   PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
   if( !pNodeData )
   {
      return;
   }

   if( pNodeData->GetItemType() != IT_PROC_INFO )
   {
      return;
   }

   PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
   if( !pProcInfo )
   {
      return;
   }

   Utils::SelectFileInExplorer( pProcInfo->csFullPath );
}// End HandleOpenParentFolder

void CProcessViewerDlg::HandleProperties()
{
   PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
   if( !pNodeData )
   {
      return;
   }
   
   if( pNodeData->GetItemType() != IT_PROC_INFO )
   {
      return;
   }
   
   PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
   if( !pProcInfo )
   {
      return;
   }

   // Show properties dialog
   Utils::ShowFilePropertiesDlg( pProcInfo->csFullPath );
}// End HandleProperties

void CProcessViewerDlg::HandlePrivilegeManager()
{
    PINodeData pNodeData = m_ctvProcess.GetItemDataOfSelectedItem();
    if( !pNodeData )
    {
        return;
    }

    if( pNodeData->GetItemType() != IT_PROC_INFO )
    {
        return;
    }

    PPROC_INFO_t pProcInfo = DCAST( PPROC_INFO_t, pNodeData );
    if( !pProcInfo )
    {
        return;
    }

    CPrivilegeMgrDlg PrivMgrDlg;
    if( PrivMgrDlg.SetProcessHandle( pProcInfo->ahmProcessHandle ))
	{
		PrivMgrDlg.DoModal();
	}
}// End HandlePrivilegeManager