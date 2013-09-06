// SearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "SearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SearchDlg::SearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SearchDlg)
	//}}AFX_DATA_INIT
}


void SearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SearchDlg)
	DDX_Control(pDX, IDC_EDIT_SRCH_STRING, m_cedtSrchStr );
	DDX_Radio(pDX, IDC_RADIO_DLL, g_Settings.m_nLastSearchChoice );
	DDX_Text(pDX, IDC_EDIT_SRCH_STRING, g_Settings.m_csLastSrchString );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SearchDlg, CDialog)
	//{{AFX_MSG_MAP(SearchDlg)
	ON_BN_CLICKED(IDC_RADIO_DLL, OnRadioDll)
	ON_BN_CLICKED(IDC_OK, OnOK)
	ON_BN_CLICKED(IDC_RADIO_PROCESS, OnRadioProcess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SearchDlg message handlers



void SearchDlg::OnOK() 
{
    // Update data into settings variables
    UpdateData( TRUE );

    CDialog::OnOK();	
}

BOOL SearchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // Update data from settings variables
    UpdateData( FALSE );

    m_cedtSrchStr.SetFocus();
    m_cedtSrchStr.SetSel( 0, -1 );
	return FALSE;
}

void SearchDlg::OnRadioDll() 
{
    m_cedtSrchStr.SetFocus();
    m_cedtSrchStr.SetSel( 0, -1 );
}

void SearchDlg::OnRadioProcess() 
{
    m_cedtSrchStr.SetFocus();
    m_cedtSrchStr.SetSel( 0, -1 );
}
