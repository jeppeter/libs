#if !defined(AFX_HEADERCTRLEX_H__5980AB97_6F05_4FFE_ADE9_0EBD7197EA8B__INCLUDED_)
#define AFX_HEADERCTRLEX_H__5980AB97_6F05_4FFE_ADE9_0EBD7197EA8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeaderCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx window

class CHeaderCtrlEx : public CHeaderCtrl
{
    // Construction
    public:
	    CHeaderCtrlEx();

    // Operations
    public:

        void SetSortOrder( const bool bAscending_i ){ m_bAscending = bAscending_i; }
        bool GetSortOrder() const { return m_bAscending; }

        void SetSortItem( const int nSortItem_i ) { m_nSortItem = nSortItem_i; }
        int  GetSortItem() const { return m_nSortItem; }
        void SetSortArrow();
        void RemoveSortArrow();
        void ToString( CString& csHeader_o, LPCTSTR lpctszSeparator_i = _T( "," ));

        BOOL SubclassWindow( HWND hWndToSubClass_i )
        {
            const BOOL bStatus = CHeaderCtrl::SubclassWindow( hWndToSubClass_i );
            SetImageList( &m_ImageList );
            return bStatus;
        }


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CHeaderCtrlEx)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CHeaderCtrlEx();

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CHeaderCtrlEx)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG
        CBitmap m_AscBitmap;
        CBitmap m_DescBitmap;
        CImageList m_ImageList;

        bool m_bAscending;
        int m_nSortItem;

	    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERCTRLEX_H__5980AB97_6F05_4FFE_ADE9_0EBD7197EA8B__INCLUDED_)
