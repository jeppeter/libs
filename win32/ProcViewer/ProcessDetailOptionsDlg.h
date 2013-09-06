#ifndef _PROCESS_DETAIL_OPTIONS_DLG_H_
#define _PROCESS_DETAIL_OPTIONS_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TabPage.h"
class ProcessDetailOptionsDlg : public CTabPage
{
// Construction
public:
	ProcessDetailOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ProcessDetailOptionsDlg)
	enum { IDD = IDD_DIALOG_PROCESS_DETAILS_OPTIONS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ProcessDetailOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void OnOK();
    void OnCancel();
    
	// Generated message map functions
	//{{AFX_MSG(ProcessDetailOptionsDlg)
	virtual BOOL OnInitDialog();
    void OnToggle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PROCESS_DETAIL_OPTIONS_DLG_H_
