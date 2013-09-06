// PrivilegeMgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessViewer.h"
#include "PrivilegeMgrDlg.h"


// CPrivilegeMgrDlg dialog

IMPLEMENT_DYNAMIC(CPrivilegeMgrDlg, CDialog)

CPrivilegeMgrDlg::CPrivilegeMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrivilegeMgrDlg::IDD, pParent)
{}

CPrivilegeMgrDlg::~CPrivilegeMgrDlg()
{}

void CPrivilegeMgrDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PRIVILEGE_COLLECTION, m_PrivilegeLstBx);
}

BEGIN_MESSAGE_MAP(CPrivilegeMgrDlg, CDialog)
    ON_CONTROL( CLBN_CHKCHANGE, IDC_LIST_PRIVILEGE_COLLECTION, OnCheckPrivilegeList )
    ON_COMMAND( ID_BUTTON_REFRESH, Refresh )
    ON_BN_CLICKED(ID_BUTTON_DISABLEALL, &CPrivilegeMgrDlg::OnBnClickedButtonDisableall)
    ON_BN_CLICKED(IDC_BUTTON_ENABLE_ALL, &CPrivilegeMgrDlg::OnBnClickedButtonEnableAll)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE_ALL, &CPrivilegeMgrDlg::OnBnClickedButtonRemoveAll)
END_MESSAGE_MAP()

BOOL CPrivilegeMgrDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    Refresh();

    return TRUE;
}

void CPrivilegeMgrDlg::Refresh()
{
    Utils::AutoSetRedrawManager asrmRedrawMgr;
    asrmRedrawMgr.AddWindow( m_PrivilegeLstBx );

    ASSERT( m_ProcessPrivMgr.GetToken() );
    m_ProcessPrivMgr.InitPrivilegeMap();

    // Remove previous entries from list box
    m_PrivilegeLstBx.ResetContent();

    const PrivilegeMap::CPair* pPair = m_ProcessPrivMgr.GetPrivilegeMap().PGetFirstAssoc();
    int nIndex = 0;
    while( pPair )
    {
        m_PrivilegeLstBx.InsertString( nIndex, pPair->value.GetPrivilegeName() + " - " + pPair->value.GetPrivilegeDescription() );
        m_PrivilegeLstBx.SetCheck( nIndex, pPair->value.GetPrivilegeEnabled() );
        m_PrivilegeLstBx.SetItemData( nIndex, RCAST( DWORD_PTR, &pPair->value ));
        ++nIndex;

        pPair = m_ProcessPrivMgr.GetPrivilegeMap().PGetNextAssoc( pPair );
    }// End while
}// End Refresh

void CPrivilegeMgrDlg::OnCheckPrivilegeList()
{
    // Get selected item index from listbox
    const int nCurSel = m_PrivilegeLstBx.GetCurSel();
    if( nCurSel == LB_ERR )
    {
        return;
    }

    // Get checked status of selected item
    const bool bItemChecked = m_PrivilegeLstBx.GetCheck( nCurSel ) == BST_CHECKED;

    // Get stored item data from list box
    CPrivilege* pPrivilegeItemData = RCAST( CPrivilege*, m_PrivilegeLstBx.GetItemData( nCurSel ));
    if( !pPrivilegeItemData )
    {
        // No item data return
        return;
    }

    // If privilege is to be enabled/disabled then enable/disable
    if( !m_ProcessPrivMgr.EnablePrivilege( pPrivilegeItemData->GetPrivilegeName(), bItemChecked ))
    {
        g_Log.Write( _T( "Failed to adjust privilege value for '%s'" ), pPrivilegeItemData->GetPrivilegeName());

		// Display last error
		Utils::GetLastErrorMsg();

        // Reverse the checking effect, since we couldn't enable the privilege for the process
        m_PrivilegeLstBx.SetCheck( nCurSel, !bItemChecked );
        return;
    }// End if
}// End OnCheckPrivilegeList

void CPrivilegeMgrDlg::OnBnClickedButtonDisableall()
{
    const int nStatus = m_ProcessPrivMgr.EnableAllPrivileges( true );
    if( nStatus < 0 )
    {
        Utils::ShowInfo( _T( "Failed to disable %d privilege(s)!" ), abs( nStatus ));
    }
    else if( nStatus > 0 )
    {
        Utils::ShowError( _T( "No privilege could be disabled, make sure you have the required access!" ));
    }// End if

	Refresh();
}
void CPrivilegeMgrDlg::OnBnClickedButtonEnableAll()
{
    const int nStatus = m_ProcessPrivMgr.EnableAllPrivileges( true );
    if( nStatus < 0 )
    {
        Utils::ShowInfo( _T( "Failed to enable %d privilege(s)!" ), abs( nStatus ));
    }
    else if( nStatus > 0 )
    {
        Utils::ShowError( _T( "No privilege could be enabled, make sure you have the required access!" ));
    }// End if

    Refresh();
}

void CPrivilegeMgrDlg::OnBnClickedButtonRemoveAll()
{
    if( Utils::ShowQuestion( _T( "Once 'removed' you won't be able to enable these back again!\nDo you wish to continue?" )) != IDYES )
    {
        return;
    }

    const int nStatus = m_ProcessPrivMgr.RemoveAllPrivileges();
    if( nStatus < 0 )
    {
        Utils::ShowInfo( _T( "Failed to remove %d privilege(s), but some were removed" ), abs( nStatus ));
    }
    else if( nStatus > 0 )
    {
        Utils::ShowError( _T( "No privilege could be removed, make sure you have the required access!" ));
    }// End if

    Refresh();
}
