#pragma once
#include "BufferedDC.h"
#include "Color.h"


// CProgressBarEx

class CProgressBarEx : public CProgressCtrl
{
	DECLARE_DYNAMIC(CProgressBarEx)

public:

	CProgressBarEx();
	virtual ~CProgressBarEx();
	
	afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    // Colors for drawing gradient rect
    Color m_clStartingColor;
    Color m_clEndingColor;

    // Back ground color
    Color m_clBkColor;

protected:

	BOOL DrawSmoothProgressBar( CDC* pDC_i );
    BOOL DrawGradientProgressRect( CDC *pDC_i, const CRect& crPrgRect_i );

	DECLARE_MESSAGE_MAP()

};// End class CProgressBarEx