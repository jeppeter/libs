#if !defined(AFX_OPTIONSDLG_H__DCE4334B_1BFE_4265_A493_21F24CAD67DD__INCLUDED_)
#define AFX_OPTIONSDLG_H__DCE4334B_1BFE_4265_A493_21F24CAD67DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//
#include "ProcessDetailOptionsDlg.h"
#include "ModuleDetailsDlg.h"
#include "GeneralOptions.h"

/////////////////////////////////////////////////////////////////////////////
// OptionsDlg dialog

class OptionsDlg : public CDialog
{
// Construction
public:

	OptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(OptionsDlg)
	enum { IDD = IDD_DIALOG_OPTIONS };
	CTabCtrl	m_Tab;
	//}}AFX_DATA

    ProcessDetailOptionsDlg m_podPrcDetailsDlg;
    ModuleDetailsDlg m_mdModuleDetailsDlg;
    GeneralOptions m_goGeneralOptionsDlg;

    void SwitchTab( const int nNewIndex_i );
    void HandleCtrlTab();
    void RepositionTab();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OptionsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    CRect m_crDisplayRect;
    void PrepareDisplayRect();

    void OnOK();

	// Generated message map functions
	//{{AFX_MSG(OptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabOption(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__DCE4334B_1BFE_4265_A493_21F24CAD67DD__INCLUDED_)
