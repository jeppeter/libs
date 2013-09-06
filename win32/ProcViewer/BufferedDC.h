// Copyright(c) Nibu babu thomas.
// Author Nibu babu thomas

#ifndef _BUFFERED_DC_H_
#define _BUFFERED_DC_H_

#pragma once

class CBufferedDC : public CDC
{
    public:

        CBufferedDC(void);
        virtual ~CBufferedDC(void);

        // Create a buffered DC
        BOOL Create( CDC *pTargetDC_i, const CRect& crTargetDCRect_i );

        // Destory created object
        void DestroyObjects();

        // Flushes memory buffer to the target DC
        BOOL Flush();

        // Inlines //
        const CDC&      GetTargetDC()       const  { return *m_pTargetDC; }
        const CBitmap&  GetBufferBmp()      const  { return m_bmpBuffer; }
        const CRect&    GetTargetDCRect()   const  { return m_crTargetDCRect; }
        // Inlines //

    protected:

        // Returns true if target dc is valid
        BOOL IsTargetDCValid() const
        { return ( m_pTargetDC && m_pTargetDC->GetSafeHdc() ); }

    private:

        // Target DC object
        CDC* m_pTargetDC;

        // Memory buffer bitmap
        CBitmap m_bmpBuffer;
		int m_nRestorePoint;

        // Buffer rectangle
        CRect m_crTargetDCRect;
};// End class CBufferedDC

#endif // _BUFFERED_DC_H_