// ProcessDetailOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "ProcessDetailOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ProcessDetailOptionsDlg dialog


ProcessDetailOptionsDlg::ProcessDetailOptionsDlg(CWnd* pParent /*=NULL*/)
	: CTabPage(ProcessDetailOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ProcessDetailOptionsDlg)
	//}}AFX_DATA_INIT
}


void ProcessDetailOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProcessDetailOptionsDlg)
	DDX_Check(pDX, IDC_CHECK_SIZE, g_Settings.m_bProcessSize);
	DDX_Check(pDX, IDC_CHECK_PROCESS_TYPE, g_Settings.m_bProcessType);
	DDX_Check(pDX, IDC_CHECK_HWND, g_Settings.m_bProcessHwnd);
	DDX_Check(pDX, IDC_CHECK_VERSION, g_Settings.m_bProcessVersion);
	DDX_Check(pDX, IDC_CHECK_MEMINFO, g_Settings.m_bProcessMemInfo);
	DDX_Check(pDX, IDC_CHECK_PROCTIMES, g_Settings.m_bProcessTimes);
	DDX_Check(pDX, IDC_CHECK_FILETIMES, g_Settings.m_bProcessFileTimes);
	DDX_Check(pDX, IDC_CHECK_GDIINFO, g_Settings.m_bProcessGDIResInfo);
    DDX_Check(pDX, IDC_CHECK_PROCESSPRIORITY, g_Settings.m_bProcessPriority);
    DDX_Check(pDX, IDC_CHECK_PROCESSIOCOUNTERS, g_Settings.m_bProcessIOCounters);
    DDX_Check(pDX, IDC_CHECK_PROCESSPRIVILEGES, g_Settings.m_bProcessPrivileges);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ProcessDetailOptionsDlg, CTabPage)
	//{{AFX_MSG_MAP(ProcessDetailOptionsDlg)
    ON_COMMAND( IDC_BUTTON_TOGGLE, OnToggle )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProcessDetailOptionsDlg message handlers

BOOL ProcessDetailOptionsDlg::OnInitDialog() 
{
	CTabPage::OnInitDialog();

    // Update data from settings
    UpdateData( FALSE );
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ProcessDetailOptionsDlg::OnOK()
{}

void ProcessDetailOptionsDlg::OnCancel()
{}

void ProcessDetailOptionsDlg::OnToggle()
{
    g_Settings.m_bProcessSize = !g_Settings.m_bProcessSize;
    g_Settings.m_bProcessType = !g_Settings.m_bProcessType;
    g_Settings.m_bProcessHwnd = !g_Settings.m_bProcessHwnd;
    g_Settings.m_bProcessVersion = !g_Settings.m_bProcessVersion;
    g_Settings.m_bProcessMemInfo = !g_Settings.m_bProcessMemInfo;
    g_Settings.m_bProcessTimes = !g_Settings.m_bProcessTimes;
    g_Settings.m_bProcessFileTimes = !g_Settings.m_bProcessFileTimes;
    g_Settings.m_bProcessGDIResInfo = !g_Settings.m_bProcessGDIResInfo;
    g_Settings.m_bProcessPriority = !g_Settings.m_bProcessPriority;
    g_Settings.m_bProcessIOCounters = !g_Settings.m_bProcessIOCounters;
    g_Settings.m_bProcessPrivileges = !g_Settings.m_bProcessPrivileges;
    UpdateData( FALSE );
}