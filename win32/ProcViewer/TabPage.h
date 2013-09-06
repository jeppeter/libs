#ifndef _TAB_PAGE_H_
#define _TAB_PAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTabPage : public CDialog  
{
    public:
	    CTabPage( const UINT ID_i, CWnd* pParent_i );
	    virtual ~CTabPage();

    protected:
        
        // Dialog initialization
        BOOL OnInitDialog();

    private:

        // Control coloring
        HBRUSH OnCtlColor(CDC* pDC_i, CWnd* pWnd_i, UINT uCntrlType_i );
        
        // Used in OnCtlColor to return from function, created in OnInitDialog
        CBrush m_BackgroundBrush;

        DECLARE_MESSAGE_MAP()
};// End CTabPage

#endif // _TAB_PAGE_H_
