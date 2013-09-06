// Copyright(c) Nibu babu thomas.
// Author Nibu babu thomas

#include "StdAfx.h"
#include ".\buffereddc.h"

CBufferedDC::CBufferedDC(void)
:   m_pTargetDC( 0 ),
	m_nRestorePoint(0)
{}

CBufferedDC::~CBufferedDC(void)
{}

BOOL CBufferedDC::Create( CDC *pTargetDC_i, const CRect& crTargetDCRect_i )
{
    DestroyObjects(); // If already created

	// Assign buffer rect
	m_crTargetDCRect = crTargetDCRect_i;

	// Assign target dc
	m_pTargetDC = pTargetDC_i;

    if( !IsTargetDCValid() || 
        !CreateCompatibleDC( pTargetDC_i ) ||
        !m_bmpBuffer.CreateCompatibleBitmap( pTargetDC_i, m_crTargetDCRect.Width(), m_crTargetDCRect.Height() ) ||
		!SelectObject( m_bmpBuffer ))
    {
        ASSERT( FALSE );
        return FALSE;
    }// End if
	
	// Save current mem dc settings
	m_nRestorePoint = SaveDC();
	
    return TRUE;
}// End Create

void CBufferedDC::DestroyObjects()
{
	if( m_nRestorePoint )
	{
		RestoreDC( m_nRestorePoint );
		m_nRestorePoint = 0;
	}

    // Delete if already created
    if( m_bmpBuffer.GetSafeHandle())
    {
        m_bmpBuffer.DeleteObject();
    }//End if

    // Delete DC if already created
    if( GetSafeHdc())
    {
        DeleteDC();
    }// End if

}// End DestroyObjects

BOOL CBufferedDC::Flush()
{
    // Validate target DC
    if( !IsTargetDCValid() )
    {
        ASSERT( FALSE );
        return FALSE;
    }
    
    return m_pTargetDC->BitBlt( m_crTargetDCRect.left,          // x
                                m_crTargetDCRect.top,           // y
                                m_crTargetDCRect.Width(),       // width
                                m_crTargetDCRect.Height(),      // height
                                this,                           // Source dc
                                m_crTargetDCRect.left,          // Source x
                                m_crTargetDCRect.top,           // Source y
                                SRCCOPY );                      // Mode of copying
}// End Flush