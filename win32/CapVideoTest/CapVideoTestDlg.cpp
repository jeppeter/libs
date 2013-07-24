// CapVideoTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CapVideoTest.h"
#include "CapVideoTestDlg.h"
#include "WindowCapture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCapVideoTestDlg dialog

CCapVideoTestDlg::CCapVideoTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCapVideoTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCapVideoTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_InitHotKey = FALSE;
}

void CCapVideoTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCapVideoTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCapVideoTestDlg, CDialog)
	//{{AFX_MSG_MAP(CCapVideoTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_WM_DESTROY()
	ON_COMMAND(IDM_FILE_SAVE, OnFileSave)
	ON_COMMAND(IDM_OPTIONS_CAPTURE, OnOptionsCapture)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCapVideoTestDlg message handlers

BOOL CCapVideoTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    m_hBmp = NULL;

    m_objHog.SetVideo("hog.dat");
    m_objHog.Hog();

	m_InitHotKey = RegisterHotKey(this->m_hWnd,100,MOD_WIN,0x41);
	if (m_InitHotKey )
	{
		m_InitHotKey = RegisterHotKey(this->m_hWnd,101,MOD_WIN|MOD_SHIFT,0x41);
		if (!m_InitHotKey)
		{
			AfxMessageBox("Can not Register WIN+SHIFT+b");

			UnregisterHotKey(this->m_hWnd,100);
		}
	}
	else
	{
		AfxMessageBox("Can not Register WIN+b");
	}
	DEBUG_INFO("init succ\n");	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCapVideoTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCapVideoTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();

        if (m_hBmp)
        {
            HDC hDC = ::GetDC(this->m_hWnd);
            if (hDC)
            {
                HDC hMemDC = ::CreateCompatibleDC(hDC);
                if (hMemDC)
                {
                    RECT clRect;
                    this->GetClientRect(&clRect);

                    HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, this->m_hBmp);
                    ::BitBlt(hDC, 0, 0, clRect.right-clRect.left, clRect.bottom-clRect.top, hMemDC, 0, 0, SRCCOPY);
                    ::SelectObject(hMemDC, hOldBmp);
                    ::DeleteDC(hMemDC);
                }
                ::ReleaseDC(this->m_hWnd, hDC);
            }
        }
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCapVideoTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCapVideoTestDlg::OnExit() 
{
    this->DestroyWindow();
}

void CCapVideoTestDlg::OnFileSave() 
{
    // This code snipet from: Windows 2000 Graphics API Black Book.

    // initialize the picture's description
    PICTDESC pd;
    pd.cbSizeofstruct = sizeof(PICTDESC);
    pd.picType = PICTYPE_BITMAP;
    pd.bmp.hbitmap = m_hBmp;
    pd.bmp.hpal = NULL;
    BOOL    result = TRUE;

    // create the IPicture object
    LPPICTURE pPicture;
    HRESULT hRes =  OleCreatePictureIndirect(&pd,
                                            IID_IPicture,
                                            false,
                                            reinterpret_cast<void**>(&pPicture)
                                            );
    if (SUCCEEDED(hRes))
    {
        // create an IStream object
        LPSTREAM pStream;
        hRes = CreateStreamOnHGlobal(NULL, true, &pStream);
        if (SUCCEEDED(hRes))
        {
            // write the picture to the stream
            LONG bytes_streamed;
            hRes = pPicture->SaveAsFile(pStream, true, &bytes_streamed);
            if (SUCCEEDED(hRes))
            {
                // write the stream to the file
                HANDLE hFile =  CreateFile("out.bmp",
                                            GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
                                            );
                if (hFile)
                {
                    HGLOBAL hMem = NULL;
                    GetHGlobalFromStream(pStream, &hMem);
                    LPVOID lpData = GlobalLock(hMem);

                    DWORD bytes_written;
                    result = WriteFile(hFile,
                                        lpData, bytes_streamed,
                                        &bytes_written, NULL
                                        );
                    result &= (bytes_written == static_cast<DWORD>(bytes_streamed));

                    GlobalUnlock(hMem);
                    CloseHandle(hFile);
                }
            }
            pStream->Release();
        }
        pPicture->Release();
    }

    if (result)
    {
        MessageBox("Save to out.bmp");
    }
    else
    {
        MessageBox("Fail to save to file");
    }

    return;
}

#define MYTIMER 1

void CCapVideoTestDlg::OnDestroy() 
{
	CDialog::OnDestroy();

    this->KillTimer(MYTIMER);

    if (m_hBmp)
    {
        ::DeleteObject(m_hBmp);
        m_hBmp = NULL;
    }
	if (m_InitHotKey)
	{
		UnregisterHotKey(this->m_hWnd,100);
		UnregisterHotKey(this->m_hWnd,101);
	}
	m_InitHotKey = FALSE;
}

void CCapVideoTestDlg::OnOptionsCapture() 
{
    CMenu *pMenu = this->GetMenu();

    if (pMenu->GetMenuState(IDM_OPTIONS_CAPTURE, MF_BYCOMMAND) == MF_CHECKED)
    {
        pMenu->CheckMenuItem(IDM_OPTIONS_CAPTURE, MF_UNCHECKED | MF_BYCOMMAND);
        this->KillTimer(MYTIMER);
    }
    else
    {
        pMenu->CheckMenuItem(IDM_OPTIONS_CAPTURE, MF_CHECKED | MF_BYCOMMAND);
        this->SetTimer(MYTIMER, 2000, NULL);
    }
}

void CCapVideoTestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    if (nIDEvent == MYTIMER)
    {
        if (m_hBmp)
        {
            ::DeleteObject(m_hBmp);
            m_hBmp = NULL;
        }

        //m_hBmp = CaptureForegroundWindow(FALSE);
		m_hBmp = CaptureDesktop();

        this->PostMessage(WM_PAINT);
    }

	CDialog::OnTimer(nIDEvent);
}

LRESULT CCapVideoTestDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
		CString mstr;

        if (m_hBmp)
        {
            ::DeleteObject(m_hBmp);
            m_hBmp = NULL;
        }

		mstr.Format("wparam 0x%08x lparam 0x%08x",wParam,lParam);
		//AfxMessageBox((LPCSTR)mstr);

		if (wParam == 0x64)
		{
		
		m_hBmp = CaptureDesktop();
		}
		else
		{
        m_hBmp = CaptureForegroundWindow(FALSE);
		}
        this->PostMessage(WM_PAINT);
	return 0;
}

