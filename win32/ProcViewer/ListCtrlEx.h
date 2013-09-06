#ifndef _LISTCTRL_EX_H_
#define _LISTCTRL_EX_H_

#include "HeaderCtrlEx.h"
#include "TypedefsInclude.h"


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * CListCtrlEx - Extended list control
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-31
 */
class CListCtrlEx : public CListCtrl
{
    public:

	    CListCtrlEx();
	    virtual ~CListCtrlEx();

        bool SaveState( LPCTSTR lpctszSectionName_i );
        bool LoadState( LPCTSTR lpctszSectionName_i );

    private:

	    COLORREF m_clrUERowColor;
        COLORREF m_clrSortedColumnColor;
        static int CALLBACK SortCB( LPARAM lItemIndex1_i, 
                                    LPARAM lItemIndex2_i, 
                                    LPARAM lCustomParam_i );
        LRESULT WindowProc( UINT uMessage_i, WPARAM wParam_i, LPARAM lParam_i );
        void PreSubclassWindow();

        // Handle short cut keys
        void OnCopy();
        void OnSelectAll();
        void OnToggleSelection();

        // Converts an Item to string, 
        void ItemToString( const int nItem_i, CString& csItem_o, LPCTSTR lpctszSeparator_i = _T( "," ));

    protected:

	    //{{AFX_MSG(CListCtrlEx)
	    afx_msg void OnCustomDraw( NMHDR* pNMHDR_i, LRESULT* pResult_i );
        afx_msg void OnColumnClickProcessModules( NMHDR* pNMHDR_i, LRESULT* pResult_i );
	    afx_msg void OnKeyDown( UINT uChar_i, UINT uRepCnt_i, UINT uFlags_i );
    	//}}AFX_MSG

        CHeaderCtrlEx m_Header;
        TD_StrCmpLogicalW m_StrCmpLogicalWFuncPtr;

	    DECLARE_MESSAGE_MAP()
};

#endif // _LISTCTRL_EX_H_
