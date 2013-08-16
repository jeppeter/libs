
// DIDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DIDialog.h"
#include "DIDialogDlg.h"
#include "afxdialogex.h"
#include "..\\common\\output_debug.h"
#include "..\\common\\uniansi.h"
#include "..\\common\\dllinsert.h"

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
    ON_MESSAGE(WM_HOTKEY,OnHotKey)
END_MESSAGE_MAP()



// CDIDialogDlg message handlers

BOOL CDIDialogDlg::OnInitDialog()
{
    CComboBox * pCombo=NULL;
    unsigned int i;
    CString str;
    CButton *pCheck=NULL;
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    ShowWindow(SW_SHOWDEFAULT);

    // TODO: Add extra initialization here
    /*set the select mask*/
    pCombo = (CComboBox*)this->GetDlgItem(IDC_COMBO_CHAR);
    for(i=0; i<26; i++)
    {
        str.Format(TEXT("%c"),'A'+i);
        pCombo->InsertString(i,str);
    }

    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_CTRL);
    pCheck->SetCheck(BST_UNCHECKED);
    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_ALT);
    pCheck->SetCheck(BST_UNCHECKED);
    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_WIN);
    pCheck->SetCheck(BST_UNCHECKED);
    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_SHIFT);
    pCheck->SetCheck(BST_UNCHECKED);


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

#define  CAPTURE_HOTKEY_ID      131

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
    CButton *pCheck=NULL;
    CComboBox *pCombo=NULL;
    int ctrlcheck=BST_UNCHECKED,altcheck=BST_UNCHECKED,wincheck=BST_UNCHECKED,shiftcheck=BST_UNCHECKED,charsel=-1;
    char* pCommandLine=NULL;
    int commandlinesize=0;
    CString errstr;
    UINT hkmask,hkvk;
    BOOL bret;

    /*now to get the string*/
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_EXE);
    pEdt->GetWindowText(m_strExe);
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_DLL);
    pEdt->GetWindowText(m_strDll);
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_BMP);
    pEdt->GetWindowText(m_strBmp);
    pEdt = (CEdit*) this->GetDlgItem(IDC_EDT_PARAM);
    pEdt->GetWindowText(m_strParam);

    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_CTRL);
    ctrlcheck=pCheck->GetCheck();

    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_ALT);
    altcheck=pCheck->GetCheck();
    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_WIN);
    wincheck=pCheck->GetCheck();
    pCheck = (CButton*) this->GetDlgItem(IDC_CHECK_SHIFT);
    shiftcheck=pCheck->GetCheck();

    pCombo = (CComboBox*) this->GetDlgItem(IDC_COMBO_CHAR);
    charsel = pCombo->GetCurSel();





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
	pParam = (const char*)m_strParam;
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

    if(pDllName == NULL || pFullDllName == NULL || strlen(pFullDllName) == 0)
    {
        errstr.Format(TEXT("Can not Run(%s)"),pCommandLine);
        AfxMessageBox(errstr);
        goto out;
    }

    if(ctrlcheck != BST_CHECKED && altcheck != BST_CHECKED &&
            wincheck != BST_CHECKED)
    {
        AfxMessageBox(TEXT("Ctrl Alt Win Must specify one"));
        goto out;
    }

    if(charsel == CB_ERR || charsel < 0 || charsel >= 26)
    {
        AfxMessageBox(TEXT("Must Select one char"));
        goto out;
    }


    DEBUG_INFO("exename (%s)\n",pExecName);
    DEBUG_INFO("dllname (%s)\n",pDllName);
    DEBUG_INFO("param (%s)\n",pParam);
    DEBUG_INFO("bmpfile (%s)\n",pBmpFile);
    DEBUG_INFO("FullDllName (%s)\n",pFullDllName);

    /*now to start for the running*/
    commandlinesize = strlen(pExecName);
    if(strlen(pParam))
    {
        commandlinesize += 1;
        commandlinesize += strlen(pParam);
    }

    commandlinesize += 1;

    pCommandLine = new char[commandlinesize];
    if(strlen(pParam))
    {
        ret = _snprintf_s(pCommandLine,commandlinesize,commandlinesize,"%s %s",pExecName,pParam);
    }
    else
    {
        ret = _snprintf_s(pCommandLine,commandlinesize,commandlinesize,"%s",pExecName);
    }

    ret = LoadInsert(NULL,pCommandLine,pFullDllName,pDllName);
    if(ret < 0)
    {
        errstr.Format(TEXT("Can not run(%s)"),pCommandLine);
        AfxMessageBox(errstr);
        goto out;
    }

    /*now to register hotkey ,first to unregister hotkey*/
    UnregisterHotKey(this->m_hWnd,CAPTURE_HOTKEY_ID);
    hkmask = 0;
    if(altcheck)
    {
        hkmask |= MOD_ALT;
    }
    if(ctrlcheck)
    {
        hkmask |= MOD_CONTROL;
    }
    if(wincheck)
    {
        hkmask |= MOD_WIN;
    }
    if(shiftcheck)
    {
        hkmask |= MOD_SHIFT;
    }

    /*this is for VK_A*/
    hkvk = 0x41;
    hkvk += charsel;

    bret = RegisterHotKey(this->m_hWnd,CAPTURE_HOTKEY_ID,hkmask,hkvk);
    if(!bret)
    {
        errstr.Format(TEXT("Register Hotkey Error %d"),GetLastError());
        AfxMessageBox(errstr);
        UnregisterHotKey(this->m_hWnd,CAPTURE_HOTKEY_ID);
        goto out;
    }
    /*all is ok*/

out:
    if(pCommandLine)
    {
        delete [] pCommandLine;
    }
    pCommandLine = NULL;

#ifdef _UNICODE
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


LRESULT CDIDialogDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
    if(wParam == CAPTURE_HOTKEY_ID)
    {

        /*now to get top window*/
        HWND hWnd=NULL;
        char *pDllName=NULL,*pFullDllName=NULL,*pExecName=NULL,*pBmpFile=NULL,*pParam = NULL;
        int fulldllnamesize=0,execnamesize=0,bmpfilesize=0,paramsize=0;
        unsigned int processid;
        int ret;
        CString errstr;
        hWnd = ::GetDesktopWindow();
        GetWindowThreadProcessId(hWnd,(LPDWORD)&processid);

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
		pParam = (const char*)m_strParam;

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

        ret = CaptureFile(processid,pDllName,"Capture3DBackBuffer",pBmpFile);
        if(ret != 0)
        {
            errstr.Format(TEXT("Could not capture on process[%d] in %s error (%d)"),processid,pBmpFile,ret);
            goto out;
        }
out:
#ifdef _UNICODE
        UnicodeToAnsi(NULL,&pFullDllName,&fulldllnamesize);
        UnicodeToAnsi(NULL,&pExecName,&execnamesize);
        UnicodeToAnsi(NULL,&pBmpFile,&bmpfilesize);
        UnicodeToAnsi(NULL,&pParam,&paramsize);
#endif
    }
    return 0;
}



