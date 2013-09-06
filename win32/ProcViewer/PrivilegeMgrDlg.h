#pragma once
#include "afxwin.h"

#include "ProcessPrivilegeMgr.h"


// CPrivilegeMgrDlg dialog
class CPrivilegeMgrDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrivilegeMgrDlg)
public:

	CPrivilegeMgrDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPrivilegeMgrDlg();

    bool SetProcessHandle( HANDLE hProcess )
    {
        ASSERT( hProcess );
        if( !m_ProcessPrivMgr.OpenKeepProcessToken( hProcess ))
		{
			Utils::GetLastErrorMsg();
			return false;
		}

		return true;
    }

// Dialog Data
	enum { IDD = IDD_PRIVILEGE_MGR };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    void Refresh();
    void OnCheckPrivilegeList();

private:
    CCheckListBox m_PrivilegeLstBx;
    CProcessPrivilegeMgr m_ProcessPrivMgr;

	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonDisableall();
    afx_msg void OnBnClickedButtonEnableAll();
    afx_msg void OnBnClickedButtonRemoveAll();
};