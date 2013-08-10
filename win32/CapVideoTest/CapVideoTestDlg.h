// CapVideoTestDlg.h : header file
//

#if !defined(AFX_CAPVIDEOTESTDLG_H__E6424A62_6E06_45B3_8178_C98782D0D1E7__INCLUDED_)
#define AFX_CAPVIDEOTESTDLG_H__E6424A62_6E06_45B3_8178_C98782D0D1E7__INCLUDED_

#include "HogVideo.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCapVideoTestDlg dialog

class CCapVideoTestDlg : public CDialog
{
// Construction
public:
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	HBITMAP m_hBmp;
	CHogVideo m_objHog;
	CCapVideoTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCapVideoTestDlg)
	enum { IDD = IDD_CAPVIDEOTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCapVideoTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


private:
	BOOL m_InitHotKey;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCapVideoTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExit();
	afx_msg void OnDestroy();
	afx_msg void OnFileSave();
	afx_msg void OnOptionsCapture();
	afx_msg void OnInsertDllExecute();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPVIDEOTESTDLG_H__E6424A62_6E06_45B3_8178_C98782D0D1E7__INCLUDED_)
