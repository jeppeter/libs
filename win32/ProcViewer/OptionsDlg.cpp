// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionsDlg dialog


OptionsDlg::OptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(OptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void OptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsDlg)
	DDX_Control(pDX, IDC_TAB_OPTION, m_Tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionsDlg, CDialog)
	//{{AFX_MSG_MAP(OptionsDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OPTION, OnSelchangeTabOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsDlg message handlers

BOOL OptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	VERIFY( m_podPrcDetailsDlg.Create( IDD_DIALOG_PROCESS_DETAILS_OPTIONS, this ));
    //VERIFY( m_mdModuleDetailsDlg.Create( IDD_DIALOG_MODULE_DETAILS_DIALOG, this ));
    VERIFY( m_goGeneralOptionsDlg.Create( IDD_DIALOG_GENERALOPTIONS, this ));
    m_Tab.InsertItem( 0, _T( "Process" ));
    m_Tab.InsertItem( 1, _T( "General" ));

    // Reposition tab to fit to the client rectangle
    RepositionTab();
    PrepareDisplayRect();
    SwitchTab( 0 );

	return FALSE;
}

void OptionsDlg::PrepareDisplayRect()
{
    CRect crItemRect;
    m_Tab.GetItemRect( 0, &crItemRect );
    m_Tab.GetClientRect( &m_crDisplayRect );
    m_crDisplayRect.top = crItemRect.bottom;
    m_crDisplayRect.DeflateRect( 2, 2, 4, 4 );

    ::MapWindowPoints( m_Tab.GetSafeHwnd(), this->GetSafeHwnd(), ( LPPOINT )&m_crDisplayRect, 2 ); 
}// End PrepareDisplayRect

void OptionsDlg::OnSelchangeTabOption(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	*pResult = 0;
    SwitchTab( m_Tab.GetCurSel() );
}

void OptionsDlg::SwitchTab( const int nNewIndex_i )
{
    // Hide previous dialog
    m_podPrcDetailsDlg.ShowWindow( SW_HIDE );
    m_goGeneralOptionsDlg.ShowWindow( SW_HIDE );

    // New index
    switch( nNewIndex_i )
    {
        case 0:
            m_podPrcDetailsDlg.MoveWindow( &m_crDisplayRect, FALSE );
            m_podPrcDetailsDlg.ShowWindow( SW_SHOW );
            break;
        case 1:
            m_goGeneralOptionsDlg.MoveWindow( &m_crDisplayRect, FALSE );
            m_goGeneralOptionsDlg.ShowWindow( SW_SHOW );
            break;
        default:
            return;
    }// End switch
}// End SwitchTab

void OptionsDlg::RepositionTab()
{
    CRect crClientRect;
    GetClientRect( &crClientRect );
    crClientRect.DeflateRect( 7, 10, 7, 30 );
    m_Tab.MoveWindow( &crClientRect, TRUE );
}

void OptionsDlg::OnOK()
{
    // Update data into settings
    m_podPrcDetailsDlg.UpdateData( TRUE );
    m_goGeneralOptionsDlg.UpdateData( TRUE );
    
    CDialog::OnOK();
}

BOOL OptionsDlg::PreTranslateMessage(MSG* pMsg) 
{
    //ASSERT( pMsg->message != WM_KEYDOWN || pMsg->wParam != VK_TAB );
    
    if( pMsg && 
        pMsg->message == WM_KEYDOWN && 
        pMsg->wParam == VK_TAB &&
        ( GetKeyState( VK_CONTROL ) & 0x8000 )
      )
    {
        HandleCtrlTab();
        return TRUE;
    }
	return CDialog::PreTranslateMessage(pMsg);
}

void OptionsDlg::HandleCtrlTab()
{
    const int nNextIndex = (( m_Tab.GetCurSel() + 1 ) % m_Tab.GetItemCount() );
    m_Tab.SetCurSel( nNextIndex );
    m_Tab.SetCurFocus( nNextIndex );
    SwitchTab( nNextIndex );
}