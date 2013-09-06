#if !defined(AFX_MODULEDETAILSDLG_H__77BC7B4C_2439_47E5_84E4_7C958EABBF70__INCLUDED_)
#define AFX_MODULEDETAILSDLG_H__77BC7B4C_2439_47E5_84E4_7C958EABBF70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModuleDetailsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ModuleDetailsDlg dialog

class ModuleDetailsDlg : public CDialog
{
// Construction
public:
	ModuleDetailsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ModuleDetailsDlg)
	enum { IDD = IDD_DIALOG_MODULE_DETAILS_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ModuleDetailsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void OnOK(){}
    void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(ModuleDetailsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULEDETAILSDLG_H__77BC7B4C_2439_47E5_84E4_7C958EABBF70__INCLUDED_)
