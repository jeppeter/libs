// dlldlg.cpp : implementation file

#include "stdafx.h"
#include "Dlldlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDllDlg dialog

IMPLEMENT_DYNAMIC(CDllDlg, CDialog)

CDllDlg::CDllDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDllDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnterDlg)
	//}}AFX_DATA_INIT
}

void CDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterDlg)
	DDX_Text(pDX, IDC_EDT_EXECUTE, m_strExe);
	DDX_Text(pDX, IDC_EDT_DLL, m_strDll);
	DDX_Text(pDX, IDC_EDT_PARAM, m_strParam);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDllDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterDlg message handlers
