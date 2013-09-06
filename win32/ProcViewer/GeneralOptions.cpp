// GeneralOptions.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "GeneralOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeneralOptions dialog


GeneralOptions::GeneralOptions(CWnd* pParent /*=NULL*/)
	: CTabPage(GeneralOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(GeneralOptions)
	//}}AFX_DATA_INIT
}


void GeneralOptions::DoDataExchange(CDataExchange* pDX)
{
	CTabPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GeneralOptions)
	DDX_Check(pDX, IDC_CHECK_PROMPTBEFOREKILL, g_Settings.m_bPromptBeforeKillingProcess);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GeneralOptions, CTabPage)
	//{{AFX_MSG_MAP(GeneralOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeneralOptions message handlers

BOOL GeneralOptions::OnInitDialog() 
{
	CTabPage::OnInitDialog();

    // Display setting value
    UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
