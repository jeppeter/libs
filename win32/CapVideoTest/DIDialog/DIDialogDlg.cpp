
// DIDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DIDialog.h"
#include "DIDialogDlg.h"
#include "afxdialogex.h"
#include "..\\common\\output_debug.h"
#include "..\\common\\uniansi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDIDialogDlg dialog




CDIDialogDlg::CDIDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDIDialogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#ifdef _UNICODE
	m_strExe = L"";
	m_strDll = L"";
	m_strBmp = L"";
#else
	m_strExe = "";
	m_strDll = "";
	m_strBmp = "";
#endif
}

void CDIDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDIDialogDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_BTN_LOAD,OnLoad)
END_MESSAGE_MAP()


// CDIDialogDlg message handlers

BOOL CDIDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_MAXIMIZE);

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDIDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDIDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDIDialogDlg::OnLoad()
{
	DEBUG_INFO("\n");
	/*now we should test for the job*/
	char *pDllName=NULL,*pFullDllName=NULL,*pExecName=NULL,*pBmpFile=NULL;
	int dllnamesize=0,fulldllnamesize=0,execnamesize=0,bmpfilesize=0;
	int ret;
#ifdef _UNICODE
	ret = UnicodeToAnsi(m_strDllName.c_str(),&pDllName,&dllnamesize);
	if (ret < 0)
	{
		goto out;
	}
	ret = UnicodToAnsi(m_str);
#else
#endif


out:
#ifdef _UNICODE
	UnicodeToAnsi(NULL,&pDllName,&dllnamesize);
	UnicodeToAnsi(NULL,&pFullDllName,&fulldllnamesize);
	UnicodeToAnsi(NULL,&pExecName,&execnamesize);
	UnicodeToAnsi(NULL,&pBmpFile,&bmpfilesize);
#endif
	return;
}
