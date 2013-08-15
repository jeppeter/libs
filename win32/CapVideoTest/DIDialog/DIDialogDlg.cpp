
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
    m_strParam = L"";
#else
    m_strExe = "";
    m_strDll = "";
    m_strBmp = "";
    m_strParam = "";
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
    ON_COMMAND(ID_BTN_SEL_EXE,OnSelExe)
    ON_COMMAND(ID_BTN_SEL_DLL,OnSelDll)
    ON_COMMAND(ID_BTN_SEL_BMP,OnSelBmp)
END_MESSAGE_MAP()


// CDIDialogDlg message handlers

BOOL CDIDialogDlg::OnInitDialog()
{
	CComboBox * pCombo=NULL;
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    ShowWindow(SW_SHOWDEFAULT);

    // TODO: Add extra initialization here
	/*set the select mask*/
	pCombo = (CComboBox*)this->GetDlgItem(IDC_COMBO_MASK);
	/*now to add string*/
	pCombo->AddString(TEXT("MC_CTRL"));
	pCombo->AddString(TEXT("MC_ALT"));
	pCombo->AddString(TEXT("MC_WIN"));
	pCombo->AddString(TEXT("MC_CTRL|MC_ALT"));
	pCombo->AddString(TEXT("MC_CTRL|MC_WIN"));
	pCombo->AddString(TEXT("MC_ALT|MC_WIN"));
	pCombo->AddString(TEXT("MC_CTRL|MC_SHIFT"));
	pCombo->AddString(TEXT("MC_ALT|MC_SHIFT"));
	pCombo->AddString(TEXT("MC_WIN|MC_SHIFT"));
	pCombo->AddString(TEXT("MC_CTRL|MC_ALT|MC_SHIFT"));
	pCombo->AddString(TEXT("MC_CTRL|MC_WIN|MC_SHIFT"));
	pCombo->AddString(TEXT("MC_ALT|MC_WIN|MC_SHIFT"));
	
	

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDIDialogDlg::OnPaint()
{
    if(IsIconic())
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
    char *pDllName=NULL,*pFullDllName=NULL,*pExecName=NULL,*pBmpFile=NULL,*pParam = NULL;
    int fulldllnamesize=0,execnamesize=0,bmpfilesize=0,paramsize=0;
    int ret;
    CEdit* pEdt=NULL;

    /*now to get the string*/
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_EXE);
    pEdt->GetWindowText(m_strExe);
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_DLL);
    pEdt->GetWindowText(m_strDll);
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_BMP);
    pEdt->GetWindowText(m_strBmp);
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_PARAM);
    pEdt->GetWindowText(m_strParam);

#ifdef _UNICODE
    ret = UnicodeToAnsi((wchar_t*)((LPCWSTR)m_strExe),&pExecName,&execnamesize);
    if(ret < 0)
    {
        goto out;
    }
    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)m_strDll),&pFullDllName,&fulldllnamesize);
    if(ret < 0)
    {
        goto out;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)m_strBmp),&pBmpFile,&bmpfilesize);
    if(ret < 0)
    {
        goto out;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)m_strParam),&pParam,&paramsize);
    if(ret < 0)
    {
        goto out;
    }

#else
    pExecName = (const char*)m_strExe;
    pFullDllName = (const char*) m_strDll;
    pBmpFile = (const char*) m_strBmp;

#endif
    pDllName = strrchr(pFullDllName,'\\');
    if(pDllName)
    {
        pDllName += 1;
    }
    else
    {
        pDllName = pFullDllName;
    }



    /*now we should start the command */
    if(pExecName == NULL|| strlen(pExecName) == 0)
    {
        AfxMessageBox(TEXT("Must Specify Exec"));
        goto out;
    }

    if(pBmpFile == NULL|| strlen(pBmpFile)==0)
    {
        AfxMessageBox(TEXT("Must Specify Bmp file"));
        goto out;
    }

	if (pDllName == NULL || pFullDllName == NULL || strlen(pFullDllName) == 0)
	{
		AfxMessageBox(TEXT("Must Specify Insert Dll"));
		goto out;
	}

	
    DEBUG_INFO("exename (%s)\n",pExecName);
    DEBUG_INFO("dllname (%s)\n",pDllName);
    DEBUG_INFO("param (%s)\n",pParam);
    DEBUG_INFO("bmpfile (%s)\n",pBmpFile);
    DEBUG_INFO("FullDllName (%s)\n",pFullDllName);

#ifdef _UNICODE
out:
    /*pDllName is the pointer in the pFullDllName so do not free two times*/
    UnicodeToAnsi(NULL,&pFullDllName,&fulldllnamesize);
    UnicodeToAnsi(NULL,&pExecName,&execnamesize);
    UnicodeToAnsi(NULL,&pBmpFile,&bmpfilesize);
    UnicodeToAnsi(NULL,&pParam,&paramsize);
#endif
    return;
}


void CDIDialogDlg::OnSelExe()
{
    CFileDialog fdlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_READONLY,
                     TEXT("execute files (*.exe)|*.exe||"),NULL);
    CString fname;
    CEdit* pEdt=NULL;
    if(fdlg.DoModal() == IDOK)
    {
        fname = fdlg.GetPathName();
        pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_EXE);
        pEdt->SetWindowText(fname);
    }

}

void CDIDialogDlg::OnSelDll()
{
    CFileDialog fdlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_READONLY,
                     TEXT("dynamic link library files (*.dll)|*.dll||"),NULL);
    CString fname;
    CEdit* pEdt=NULL;
    if(fdlg.DoModal() == IDOK)
    {
        fname = fdlg.GetPathName();
        pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_DLL);
        pEdt->SetWindowText(fname);
    }

}

void CDIDialogDlg::OnSelBmp()
{
    CFileDialog fdlg(TRUE,NULL,NULL,0,
                     TEXT("bmp files (*.bmp)|*.bmp||"),NULL);
    CString fname;
    CEdit* pEdt=NULL;
    if(fdlg.DoModal() == IDOK)
    {
        CString bmpstr;
        fname = fdlg.GetPathName();
        bmpstr = fname.Right(4);
        if(bmpstr != TEXT(".bmp"))
        {
            fname += TEXT(".bmp");
        }
        pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_BMP);
        pEdt->SetWindowText(fname);
    }

}



