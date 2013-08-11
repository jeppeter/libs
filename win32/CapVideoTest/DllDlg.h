// dlldlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDllDlg dialog

class CDllDlg : public CDialog
{
	DECLARE_DYNAMIC(CDllDlg)
// Construction
public:
	CDllDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterDlg)
	enum { IDD = IDD_INSERT_DLL_EXE_DIALOG };
	CString m_strExe;
	CString m_strDll;
	CString m_strParam;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEnterDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
