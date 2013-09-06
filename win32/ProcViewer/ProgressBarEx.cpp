// ProgressBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBarEx.h"
#include ".\progressbarex.h"
#include "Color.h"


// CProgressBarEx

IMPLEMENT_DYNAMIC(CProgressBarEx, CProgressCtrl)

CProgressBarEx::CProgressBarEx()
: m_clStartingColor( CLR_WHITE ),
  m_clEndingColor( CLR_MS_BLUE )
{
    m_clBkColor = ::GetSysColor( COLOR_3DFACE );
}

CProgressBarEx::~CProgressBarEx()
{}


BEGIN_MESSAGE_MAP(CProgressBarEx, CProgressCtrl)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CProgressBarEx message handlers
void CProgressBarEx::OnPaint()
{
    // DC for painting
    CPaintDC dc(this);

    // Save dc state, and restore back to previous state on destruction
    Utils::AutoDCStateHandler adshDCStateHandler( &dc );

    // Draw smooth progress bar
    if( Utils::IsValidMask( GetStyle(), PBS_SMOOTH ))
    {
        // Draw progress bar
        BOOL bDrawResult = DrawSmoothProgressBar( &dc );

        // Restore dc to previous state
        adshDCStateHandler.RestoreDCState();
        
        // If painting failed then do default painting
        if( !bDrawResult )
        {
            DefWindowProc( WM_PAINT, 0, 0 );
        }
    }
    else
    {
        // Do default painting
        DefWindowProc( WM_PAINT, 0, 0 );
    }// End if
}// End OnPaint

BOOL CProgressBarEx::OnEraseBkgnd( CDC* /*pDC_i*/ )
{
    return TRUE;
}// End OnEraseBkgnd

BOOL CProgressBarEx::DrawSmoothProgressBar( CDC* pDC_i )
{
	// Get client rect
	CRect crClientRect;
	GetClientRect( &crClientRect );

	// Prepare memory DC object
	CBufferedDC dcMemoryDC;	

    // Create a memory bitmap
	if( !dcMemoryDC.Create( pDC_i, crClientRect ))
	{
		return FALSE;
	}

    // Get default control font that has been set
    CFont *pFont = GetParent()->GetFont();
    if( pFont )
    {
        // Select this font if valid
        dcMemoryDC.SelectObject( pFont );
    }

	int nLower = 0; // Lower bound
	int nUpper = 0; // Upper bound

	// Get upper and lower range bound
	GetRange( nLower, nUpper );

	// Range difference
	const int nRangeDiff = nUpper - nLower;

	// Get current progress
	const int nCurrentPos = GetPos();

	// Get percentage
	const int nPercentage = SCAST( int, ( SCAST( float, nCurrentPos ) 
                                                       / 
                                          SCAST( float, nRangeDiff )) * 100.0f );

	// Convert to string
	CString csPercentage;
	csPercentage.Format( _T( "%d%%" ), nPercentage );

	// Set background transparent
	dcMemoryDC.SetBkMode( TRANSPARENT );

    CRect crPrgRect = crClientRect;
    crPrgRect.right = SCAST( LONG, 
                           ( SCAST( float, nPercentage ) / 100.0f ) * crPrgRect.Width() );

    // Fill background
    dcMemoryDC.FillSolidRect( &crClientRect, m_clBkColor );

    // Draw gradient rectangle
    if( !DrawGradientProgressRect( &dcMemoryDC, crPrgRect ))
    {
        return FALSE;
    }

	// Draw string
	if( !dcMemoryDC.DrawText( csPercentage, crClientRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER ))
    {
        return FALSE;
    }

	// Copy bitmap to target DC
	return dcMemoryDC.Flush();
}


BOOL CProgressBarEx::DrawGradientProgressRect( CDC *pDC_i, const CRect& crPrgRect_i )
{
    if( !Utils::DrawGradient( pDC_i, crPrgRect_i, m_clStartingColor, m_clEndingColor, false ))
    {
        // Draw progress rectangle
        pDC_i->Draw3dRect( crPrgRect_i, m_clStartingColor, m_clEndingColor );
        return TRUE;
    }

    return TRUE;
}