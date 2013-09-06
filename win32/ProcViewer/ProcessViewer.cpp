// ProcessViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ProcessViewer.h"
#include "ProcessViewerDlg.h"
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Log g_Log;
/////////////////////////////////////////////////////////////////////////////
// CProcessViewerApp

BEGIN_MESSAGE_MAP(CProcessViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CProcessViewerApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessViewerApp construction

CProcessViewerApp::CProcessViewerApp()
{
	const DWORD dwBuffSize = MAX_PATH * 2;
	TCHAR szModuleFileName[dwBuffSize] = { 0 };

	// Get exe name
	GetModuleFileName( 0, szModuleFileName, dwBuffSize );

	// Remove file name part
	PathRemoveFileSpec( szModuleFileName );

	// Concatenate file name
	_tcscat_s( szModuleFileName, dwBuffSize, _T( "\\PVLog.log" ));
	VERIFY( g_Log.Open( szModuleFileName ));
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CProcessViewerApp object

CProcessViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CProcessViewerApp initialization

BOOL CProcessViewerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
#if _MSC_VER < 1400
	Enable3dControls();			// Call this when using MFC in a shared DLL
#endif
#else
#if _MSC_VER < 1400
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	if( !IsUserAnAdmin() )
	{
		Utils::ShowWarning( _T( "You are not an administrator, some features won't be accessible!" ));
	}

	ChangeProcessPriority();
	CProcessViewerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

void CProcessViewerApp::ChangeProcessPriority()
{
	HANDLE hProcessViewerProc = GetCurrentProcess();
	SetPriorityClass( hProcessViewerProc, ABOVE_NORMAL_PRIORITY_CLASS );
	SetProcessPriorityBoost( hProcessViewerProc, FALSE );
}