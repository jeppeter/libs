#ifndef _TREE_CTRL_EX_H_
#define _TREE_CTRL_EX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INodeData.h"

class CTreeCtrlEx : public CTreeCtrl
{

// Construction
public:

	CTreeCtrlEx();
    virtual ~CTreeCtrlEx();

    HTREEITEM InsertItemEx( LPCTSTR lpctszItemText_i, 
                            const int nImage_i, 
                            const int nSelectedImage_i, 
                            ITEM_TYPE_e eItemType_i = IT_NONE, 
                            PINodeData pNodeData_i = 0,
                            HTREEITEM hParent_i = TVI_ROOT );

    // Helper function
    HTREEITEM InsertItemEx( LPCTSTR lpctszItemText_i,
                            const int nImage_i,
                            const int nSelectedImage_i,
                            HTREEITEM hItemParent_i )
    {
        return InsertItemEx( lpctszItemText_i, nImage_i, nSelectedImage_i, IT_NONE, 0, hItemParent_i );
    }

    PINodeData GetItemData( HTREEITEM hItem_i )
    {
        return RCAST( PINodeData, CTreeCtrl::GetItemData( hItem_i ));
    }

    // Returns item data of selected item
    PINodeData GetItemDataOfSelectedItem()
    {
        HTREEITEM hSelItem = GetSelectedItem();
        if( !hSelItem )
        {
            return 0;
        }

        return GetItemData( hSelItem );
    }

    HTREEITEM GetItemUnderCursor();
    PINodeData GetItemDataOfItemUnderCursor()
    {
        return GetItemData( GetItemUnderCursor() );
    }

    // Item data
    BOOL ClearItemDataOf( HTREEITEM hItem_i );

    // Get immediate child count
    int GetImmediateChildrenCount( HTREEITEM hItem_i );

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlEx)
		// NOTE - the ClassWizard will add and remove member functions here.
    afx_msg void OnKeyDown( UINT uChar_i, UINT uRepCnt_i, UINT uFlags_i );
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:

    void OnCopy();
    
    // Disable external calls to these functions
    using CTreeCtrl::InsertItem;
    using CTreeCtrl::SetItemData;
    using CTreeCtrl::GetItemData;
};

#endif // _TREE_CTRL_EX_H_