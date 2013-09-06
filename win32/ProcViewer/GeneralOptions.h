#if !defined(AFX_GENERALOPTIONS_H__8B974ABF_85B6_46AB_9BD3_A30EF89FA6A2__INCLUDED_)
#define AFX_GENERALOPTIONS_H__8B974ABF_85B6_46AB_9BD3_A30EF89FA6A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TabPage.h"

class GeneralOptions : public CTabPage
{
// Construction
public:
	GeneralOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GeneralOptions)
	enum { IDD = IDD_DIALOG_GENERALOPTIONS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GeneralOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GeneralOptions)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALOPTIONS_H__8B974ABF_85B6_46AB_9BD3_A30EF89FA6A2__INCLUDED_)
