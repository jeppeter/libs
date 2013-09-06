// ModuleDetailsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "ModuleDetailsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ModuleDetailsDlg dialog


ModuleDetailsDlg::ModuleDetailsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ModuleDetailsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ModuleDetailsDlg)
	//}}AFX_DATA_INIT
}


void ModuleDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ModuleDetailsDlg)
	DDX_Check(pDX, IDC_CHECK_COMPANY, g_Settings.m_bModuleCompany);
	DDX_Check(pDX, IDC_CHECK_DESCRIPTION, g_Settings.m_bModuleDescription);
	DDX_Check(pDX, IDC_CHECK_ENTRYPOINT, g_Settings.m_bModuleEntryPoint);
	DDX_Check(pDX, IDC_CHECK_FILESIZE, g_Settings.m_bModuleFileSize);
	DDX_Check(pDX, IDC_CHECK_IMAGESIZE, g_Settings.m_bModuleImageSize);
	DDX_Check(pDX, IDC_CHECK_INDEX, g_Settings.m_bModuleIndex);
	DDX_Check(pDX, IDC_CHECK_LOADADDRESS, g_Settings.m_bModuleLoadAddress);
	DDX_Check(pDX, IDC_CHECK_MODULENAME, g_Settings.m_bModuleName);
	DDX_Check(pDX, IDC_CHECK_MODULEPATH, g_Settings.m_bModulePath);
	DDX_Check(pDX, IDC_CHECK_VERSION, g_Settings.m_bModuleVersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ModuleDetailsDlg, CDialog)
	//{{AFX_MSG_MAP(ModuleDetailsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ModuleDetailsDlg message handlers

BOOL ModuleDetailsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // Transfer data from settings
    UpdateData( FALSE );
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
