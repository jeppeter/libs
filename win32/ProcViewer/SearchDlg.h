#if !defined(AFX_SEARCHDLG_H__2075AE4C_B2F6_4C21_93ED_25E965F63356__INCLUDED_)
#define AFX_SEARCHDLG_H__2075AE4C_B2F6_4C21_93ED_25E965F63356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchDlg.h : header file
//

enum SRCH_TYPE_e
{
    ST_DLL = 10,
    ST_PROCESS
};

/**
 *
 * SEARCH_CRITERIA - Search criteria structure.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-13
 */
typedef struct SEARCH_CRITERIA
{
    SEARCH_CRITERIA() : eSrchType( ST_PROCESS )
    {}
    
    CString csSrch;
    SRCH_TYPE_e eSrchType;
}*PSEARCH_CRITERIA;

/////////////////////////////////////////////////////////////////////////////
// SearchDlg dialog

class SearchDlg : public CDialog
{
// Construction
public:
	SearchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SearchDlg)
	enum { IDD = IDD_DIALOG_SEARCH };
	CEdit	m_cedtSrchStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SearchDlg)
	afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioDll();
	afx_msg void OnRadioProcess();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHDLG_H__2075AE4C_B2F6_4C21_93ED_25E965F63356__INCLUDED_)
