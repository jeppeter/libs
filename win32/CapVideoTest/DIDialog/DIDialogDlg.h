
// DIDialogDlg.h : header file
//

#pragma once

#define  SNAPSHOT_TIME_ID   131

// CDIDialogDlg dialog
class CDIDialogDlg : public CDialogEx
{
// Construction
public:
	CDIDialogDlg(CWnd* pParent = NULL);	// standard constructor
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	CString m_strExe;
	CString m_strDll;
	CString m_strBmp;
	CString m_strParam;

// Dialog Data
	enum { IDD = IDD_DIDIALOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	int SnapShort();
	int SnapShot();
private:
	DWORD m_CallProcessId;
	int m_BmpId;
	int m_SnapSecond;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoad();
	afx_msg void OnSelExe();
	afx_msg void OnSelDll();
	afx_msg void OnSelBmp();
	afx_msg void OnTimer(UINT nEvent);
	DECLARE_MESSAGE_MAP()
};
