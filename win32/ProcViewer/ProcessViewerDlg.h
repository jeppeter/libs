/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * ProcessViewerDlg.h - Process viewer dialog header file.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-05
 */


#ifndef _PROCESS_VIEWER_DLG_H_
#define _PROCESS_VIEWER_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DividerWnd.h"
#include "Psapi.h"
#include "SearchDlg.h"
#include "FileVersionInfo.h"
#include "ListCtrlEx.h"
#include "afxtempl.h"
#include "WindowCollection.h"
#include "TreeCtrlEx.h"
#include "Performance.h"
#include "ProgressBarEx.h"
#include "ProcessSymbolCollection.h"
#include "ProcessCollection.h"
#include "AutoStartupInfo.h"
#include "PrivilegeMgrDlg.h"

// Modules
#define MAX_MODULES 1024


//wHandleType & 0x1F  HGDIOBJ 
//0x01  hDC 
//0x04  hRegion 
//0x05  hBitmap 
//0x08  hPalette 
//0x0A  hFont 
//0x10  hBrush 
//all other  unknown 

#define GDI_TABLE_MAX_ENTRIES 16384

// 16 bytes large
#pragma pack(1)
typedef struct
{
    DWORD dwHandle; // must be >0x80000000 
    WORD wProcessIdVersion5;   
    WORD wProcessIdVersion4;   
    WORD wUnknown;   
    WORD wHandleType; // mask with 0x1F 
    DWORD dwUnknown;   
} GDI_TABLE_ENTRY_STRUCT, *PGDI_TABLE_ENTRY_STRUCT;
#pragma pack()

typedef GDI_TABLE_ENTRY_STRUCT *(WINAPI *PFN_GDI_QUERY_TABLE_FUNCTION)( void );


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * MODULEINFO_LIST_t - TODO: Add Class Description Here.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-06
 */
typedef struct MODULEINFO_LIST_t
{
    MODULEINFO_LIST_t() : pstNextModuleInfo( 0 )
    {
        ::ZeroMemory( &stmiModInfo, sizeof( stmiModInfo ));
    }

    ~MODULEINFO_LIST_t()
    {
        delete pstNextModuleInfo;
        pstNextModuleInfo = 0;
    }

    FileVersionInfo fviVersion;
    CString csModuleFullPath;
    CString csModuleBaseName;
    MODULEINFO stmiModInfo;
    MODULEINFO_LIST_t* pstNextModuleInfo;

}*PMODULEINFO_LIST_t;


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * PROC_INFO_t - Holds process information for a node
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-07
 */
struct PROC_INFO_t : public INodeData
{
    PROC_INFO_t() : dwGDIObjs( 0 ),
                    dwUserObjs( 0 ),
                    dwProcId( 0 ), 
                    pstModuleInfoList( 0 ),
                    hProcessModule( 0 ),
                    bProcessKilled( false ),
                    uProcessPriority( NORMAL_PRIORITY_CLASS ),
                    bPriorityBoost( FALSE )
    {
        ZeroMemory( &stpmcMemCounters, sizeof( stpmcMemCounters ));
    }

    ~PROC_INFO_t()
    {
        Clear();
    }

    void Clear()
    {
        pstModuleInfoList && delete pstModuleInfoList;
        pstModuleInfoList = 0;
    }

    bool DeleteNodeData() const { return true; }

    // GDI object count
    DWORD dwGDIObjs;
    DWORD dwUserObjs;

    // Console, windows or ms-dos
    CString csApplicationType;
    
    // Process io counters
    CString csReadOprCount;
    CString csWriteOprCount;
    CString csOtherOprCount;
    CString csReadTrnsCount;
    CString csWriteTrnsCount;
    CString csOtherTrnsCount;

    // Process features
    CString csProcessStartTime;
    CString csExitTime;
    CString csKernelTime;
    CString csUserTime;
    CString csPriority;

    UINT uProcessPriority;
    BOOL bPriorityBoost;

    FileVersionInfo fviVersion;
    CString csFullPath;
    CString csBaseName;
    DWORD dwProcId;
    PROCESS_MEMORY_COUNTERS stpmcMemCounters;

    // Process handle
    Utils::AutoHandleMgr ahmProcessHandle;
    HMODULE hProcessModule;

    // Module information
    MODULEINFO stmiModInfo;

    // Set to true when process is killed
    bool bProcessKilled;

    PMODULEINFO_LIST_t pstModuleInfoList;
    ProcessSymbolCollection pscSymbolCollection;
};

typedef PROC_INFO_t* PPROC_INFO_t;


class CProcessViewerDlg : public CDialog
{
// Construction
public:
	CProcessViewerDlg(CWnd* pParent = NULL);	// standard constructor

    ~CProcessViewerDlg();

// Dialog Data
	//{{AFX_DATA(CProcessViewerDlg)
	enum { IDD = IDD_PROCESSVIEWER_DIALOG };
	CListCtrlEx	m_clvModuleSymbolList;
	CProgressBarEx	m_PrgCtrl;
	CTreeCtrlEx	m_ctvProcessDetails;
	CListCtrlEx	m_clvProcessModules;
	CTreeCtrlEx	m_ctvProcess;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

    void RefreshProcessInformation( PSEARCH_CRITERIA pscCriteria_i );
    bool GetProcessDetail( const DWORD dwProcessID_i, 
                           PROC_INFO_t& stpiProcInfo_i, 
                           PSEARCH_CRITERIA pSrchCriteria_i );
    void ModifyEnvPATH() const;
    void SetUpAccess() const;
    void UpdateProgressText( LPCTSTR lpctszProgressText_i );
    void UpdateProgressBar( const int nProgress_i );
    void SetUpProgressBar();
    void ReInitProgressBar( const int nMax_i, int nCurrentPos_i = 0 );
    void ParseOutFileName( CString& csFileName_io ) const;
    HICON GetAssociatedFileIcon( LPCTSTR lpctszFilePath_i );
    bool GetExeType( LPCTSTR lpctszFilePath_i, CString& csFileType_o );

    // Functions for updating tree information
    void LoadWindowDetailsForProcess( PROC_INFO_t& stpiProcInfo_i );
    void AddWindowDetailsToTree( HTREEITEM hItem, PWindow pWindow_i );
    void AddWindowToTree( HTREEITEM hItem, PWindow pWindow_i );
    void LoadProcessTimesForProcess( PROC_INFO_t& stpiProcInfo_i );
    void LoadFileTimesForProcess( PROC_INFO_t& stpiProcInfo_i );
    void LoadIOCounters( PROC_INFO_t& stpiProcInfo_i );
    void LoadMemoryDetailsForProcess( PROC_INFO_t& stpiProcInfo_i );
    void LoadVersionInfoDetailsForProcess( PROC_INFO_t& stpiProcInfo_i );
    void LoadModuleInformationForProcess( PROC_INFO_t& stpiProcInfo_i );
    void LoadGDIInformation( PROC_INFO_t& stpiProcInfo_i );
    void LoadProcessPrivileges( PROC_INFO_t& stpiProcInfo_i );

    // Load module symbol information
    bool LoadModuleSymbolInformation( ProcessSymbolCollection& pstSymColl_i,
                                      const DWORD dwBaseOfDll_i,
                                      LPCTSTR lpctszModuleFileName_i,
                                      const DWORD dwModuleSize_i );

    // Load modules for a process
    bool GetProcessModulesInfo( HANDLE& hProcess_i,
                                PROC_INFO_t& stProcInfo_i,
                                HMODULE* hProcModuleArray_i, 
                                PSEARCH_CRITERIA pSrchCriteria_i );

    bool GetDeviceDrivers( PROC_INFO_t& stProcInfo_i,
                           PSEARCH_CRITERIA pSrchCriteria_i );

    bool PrepareGDIInfoFunction();
    void GetProcessTimes( PROC_INFO_t& stpiProcInfo_i );
    void GetProcessPriority( HANDLE& hProcess_i, CString& csPriority_o, UINT& uProcessPriority_o );
    void GetProcessIOCounters( PROC_INFO_t& stpiProcInfo_i );


    void HandleProcessPriorityUpdateCmdUI( CCmdUI* pCmdUI_i, const UINT uCmdPriority_i );
    void HandleProcessPriorityCommand( UINT uCmdId_i );

    void HandleBoostPriorityCommand();
    void HandleOpenParentFolder();
    void HandleProperties();
    void HandlePrivilegeManager();

// Implementation
protected:
    
    Utils::AutoHICONMgr m_ahmBigIcon;
    Utils::AutoHICONMgr m_ahmSmIcon;

    HMODULE m_hGDI32dll;
    PFN_GDI_QUERY_TABLE_FUNCTION m_GDIFunc;
    
    DividerWnd m_MainDivider;
    DividerWnd m_ProcessDivider;
    DividerWnd m_ModuleDivider;

    // File icons
    CList<HICON, HICON> m_lstExtraIcons;
    
	// Generated message map functions
	//{{AFX_MSG(CProcessViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize( UINT uType, int nCx_i, int nCy_i );
	afx_msg void OnSelchangedTreeProcess(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDestroy();
	afx_msg void OnOptionsRefresh();
	afx_msg void OnOptionsSwaplayout();
	afx_msg void OnOptionsExit();
	afx_msg void OnOptionsAbout();
	afx_msg void OnOptionsShowpath();
	afx_msg void OnDblclkListProcessdetails(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeProcess(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRClkTreeProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOptionsEnabledepends();
	afx_msg void OnOptionsSearch();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnOptionsSettings();
	afx_msg void OnOptionsKillprocess();
   afx_msg void OnOptionsKillprocessAllInstances();
	afx_msg void OnItemchangedListProcessmodules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetInfoTipTreeProcess(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

    void SetupControlbars();
    CToolBar m_ToolBar;
    CStatusBar m_StatusBar;
    WindowCollection m_WC;
    ProcessCollection m_pcProcColl;
    Performance m_Perf;
    CAutoStartupInfo m_AutoStartupInfo;
    
    BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
    void UpdateModuleCount( const int nCount_i );
    void UpdateProcessCount( const int nCount_i );
    BOOL PreTranslateMessage( MSG* pMsg );
    void UpdateCount();
    void BeginPerformance();
    void EndPerformance();
    void HandleModuleListSelectionChanged( const int nItem_i );

    HACCEL m_hAccel;
    bool m_bSearchedInPath;
    bool m_bLoadedDrivers;
    CImageList m_ImageList;
    CBitmap m_Bitmap;
    CString m_csDependsPath;
    int m_nInitialImageCount;

    void OnOK(){}

    CString m_csComputerName;
    CString m_csUserName;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PROCESS_VIEWER_DLG_H_
