// CapVideoTest.h : main header file for the CAPVIDEOTEST application
//

#if !defined(AFX_CAPVIDEOTEST_H__4817FC39_FA2A_4940_A301_AB5D99F10B3D__INCLUDED_)
#define AFX_CAPVIDEOTEST_H__4817FC39_FA2A_4940_A301_AB5D99F10B3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCapVideoTestApp:
// See CapVideoTest.cpp for the implementation of this class
//

class CCapVideoTestApp : public CWinApp
{
public:
	CCapVideoTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCapVideoTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCapVideoTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPVIDEOTEST_H__4817FC39_FA2A_4940_A301_AB5D99F10B3D__INCLUDED_)
