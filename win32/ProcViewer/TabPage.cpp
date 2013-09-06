#include "stdafx.h"
#include "TabPage.h"
#include <Uxtheme.h>
#include "TypedefsInclude.h"
#include "APIMgr.h"

//************************************
// Method:    CTabPage
// FullName:  CTabPage::CTabPage
// Access:    public 
// Returns:   
// Qualifier: : CDialog( ID_i, pParent_i )
// Parameter: const UINT ID_i
// Parameter: CWnd * pParent_i
//************************************
CTabPage::CTabPage( const UINT ID_i, CWnd* pParent_i )
    : CDialog( ID_i, pParent_i )
{}


//************************************
// Method:    ~CTabPage
// FullName:  CTabPage::~CTabPage
// Access:    public 
// Returns:   
// Qualifier:
//************************************
CTabPage::~CTabPage()
{}

BEGIN_MESSAGE_MAP(CTabPage, CDialog)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


//************************************
// Method:    OnInitDialog
// FullName:  CTabPage::OnInitDialog
// Access:    private 
// Returns:   BOOL
// Qualifier:
//************************************
BOOL CTabPage::OnInitDialog()
{
    // Get function pointer
    const TD_IsAppThemed IsAppThemedPtr = g_APIMgr.GetProcAddress( "IsAppThemed", _T( "UxTheme.dll" ));

    // Get color for background of dialog
    const COLORREF ClrDCColor = GetSysColor( IsAppThemedPtr && IsAppThemedPtr() ? COLOR_WINDOW : COLOR_BTNFACE );
    m_BackgroundBrush.Attach( CreateSolidBrush( ClrDCColor ));
    return CDialog::OnInitDialog();
}

//************************************
// Method:    OnCtlColor
// FullName:  CTabPage::OnCtlColor
// Access:    private 
// Returns:   HBRUSH
// Qualifier:
// Parameter: CDC * pDC_i
// Parameter: CWnd * pWnd_i
// Parameter: UINT uCntrlType_i
//************************************
HBRUSH CTabPage::OnCtlColor( CDC* pDC_i, CWnd* pWnd_i, UINT uCntrlType_i )
{
    // First do default always
    HBRUSH hBrTemp = CDialog::OnCtlColor( pDC_i, pWnd_i, uCntrlType_i );

    // If we have a static control or a dialog control we should return our brush
    // with the DC having a transparent background
    switch( uCntrlType_i )
    {
    case CTLCOLOR_STATIC:
	case CTLCOLOR_DLG:
		hBrTemp = m_BackgroundBrush; // Our brush
		pDC_i->SetBkMode( TRANSPARENT ); // Transparent mode
    }

    // Return brush to caller
    return hBrTemp;
}