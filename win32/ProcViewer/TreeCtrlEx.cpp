// TreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "TreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx()
{
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
        ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers

HTREEITEM CTreeCtrlEx::InsertItemEx( LPCTSTR lpctszItemText_i, 
                                     int nImage_i, 
                                     int nSelectedImage_i, 
                                     ITEM_TYPE_e eItemType_i /*= IT_NONE*/, 
                                     PINodeData pNodeData_i /*= 0*/,
                                     HTREEITEM hParent_i /*= TVI_ROOT */)
{
    HTREEITEM hNewItem = CTreeCtrl::InsertItem( lpctszItemText_i, nImage_i, nSelectedImage_i, hParent_i );
    if( hNewItem && eItemType_i != IT_NONE )
    {
        // If item type is not none then node data should not be NULL
        ASSERT( pNodeData_i );
        pNodeData_i->eItemType = eItemType_i;
        SetItemData( hNewItem, RCAST( DWORD, pNodeData_i ));
    }

    // Return inserted new item
    return hNewItem;
}

// Returns item under cursor
HTREEITEM CTreeCtrlEx::GetItemUnderCursor()
{
    TVHITTESTINFO tvHitTest = { 0 };

    // Get cursor position
    ::GetCursorPos( &tvHitTest.pt );
    // Convert to client co-ordinates
    GetDesktopWindow()->MapWindowPoints( this, &tvHitTest.pt, 1 );

    return HitTest( &tvHitTest );
}


BOOL CTreeCtrlEx::ClearItemDataOf( HTREEITEM hItem_i )
{
    // Check item
    if( !hItem_i )
    {
        return FALSE;
    }

    // Special case for root item
    if( hItem_i == TVI_ROOT )
    {
        hItem_i = GetRootItem();
    }


    // Proceed towards siblings
    while( hItem_i )
    {
        PINodeData pNodeData = GetItemData( hItem_i );
        pNodeData && pNodeData->DeleteNodeData() && delete pNodeData;

        // Clear item data
        SetItemData( hItem_i, 0 );

        // Recursively Clear item data of children
        ClearItemDataOf( GetNextItem( hItem_i, TVGN_CHILD ));

        // Get next sibling
        hItem_i = GetNextItem( hItem_i, TVGN_NEXT );
    }// End while

    return TRUE;
}

int CTreeCtrlEx::GetImmediateChildrenCount( HTREEITEM hItem_i )
{
    int nCount = 0;

    // Loop through and get the kids
    HTREEITEM hChild = GetNextItem( hItem_i, TVGN_CHILD );
    while( hChild )
    {
        ++nCount;
        hChild = GetNextItem( hChild, TVGN_NEXT );
    }

    // Return count of all children
    return nCount;
}

LRESULT CTreeCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == TVM_DELETEITEM )
    {
        ClearItemDataOf( RCAST( HTREEITEM, lParam ));
    }

	return CTreeCtrl::WindowProc(message, wParam, lParam);
}


/** 
 * 
 * Key down event handler.
 * 
 * @param       uChar_i   - Char pressed
 * @param       uRepCnt_i - Repetition count
 * @param       uFlags_i  - Event flags
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CTreeCtrlEx::OnKeyDown( UINT uChar_i, UINT uRepCnt_i, UINT uFlags_i )
{
    CTreeCtrl::OnKeyDown( uChar_i, uRepCnt_i, uFlags_i );

    // We are processing only ascii keys
    if( !isascii( uChar_i ))
    {
        return;
    }
    
    // Handle special key combinations
    if(( uChar_i == 'c' || uChar_i == 'C' ) && Utils::IsCtrlDown() )
    {
        // Handle Ctrl + C
        OnCopy();
    }
}

/** 
 * 
 * Fired when Ctrl + C is pressed. See WindowProc
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CTreeCtrlEx::OnCopy()
{
    HTREEITEM hSelectedItem = GetSelectedItem();
    if( !hSelectedItem )
    {
        hSelectedItem = GetItemUnderCursor();
        if( !hSelectedItem )
        {
            return;
        }
    }// End if

    // Copy text to clipboard
    VERIFY( Utils::CopyTextToClipboard( GetSafeHwnd(), GetItemText( hSelectedItem )));
}// End OnCopy
