#include "stdafx.h"
#include "resource.h"
#include "ListCtrlEx.h"
#include <ShLWAPI.h>
#include "APIMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CListCtrlEx::CListCtrlEx() : m_clrUERowColor( RGB( 240, 240, 240 )),
                             m_clrSortedColumnColor( RGB( 250, 250, 250 )),
                             m_StrCmpLogicalWFuncPtr( 0 )
{
    // Get function pointer
    m_StrCmpLogicalWFuncPtr = ( TD_StrCmpLogicalW )g_APIMgr.GetProcAddress( "StrCmpLogicalW", _T( "ShlWApi.dll" ));
}

CListCtrlEx::~CListCtrlEx()
{}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
    //{{AFX_MSG_MAP(CListCtrlEx)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_NOTIFY_REFLECT( LVN_COLUMNCLICK, OnColumnClickProcessModules )
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CListCtrlEx::PreSubclassWindow()
{
    m_Header.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() );
    CListCtrl::PreSubclassWindow();
}

void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* plvCstDrawPtr = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;
    if( plvCstDrawPtr->nmcd.dwDrawStage == CDDS_PREPAINT )
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if( plvCstDrawPtr->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
    {
        // Alternate color painting
        if( plvCstDrawPtr->nmcd.dwItemSpec & 1 )
        {
            plvCstDrawPtr->clrTextBk = m_clrUERowColor;
        }
        else
        {
            plvCstDrawPtr->clrTextBk = RGB( 255, 255, 255);
        }// End if

        plvCstDrawPtr->clrText = RGB( 0, 0, 0 );
        *pResult = 0;
    }// End if
}// End OnCustomDraw


/** 
 * 
 * Saves column order and width to settings.
 * 
 * @param       Nil
 * @return      bool - Returns execution state
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool CListCtrlEx::SaveState( LPCTSTR lpctszSectionName_i )
{
    // Get count of columns
    const int nColumnCount = m_Header.GetItemCount();

    // Allocate memory
    int* pnColumnData = new int[nColumnCount];

    // Get order
    GetColumnOrderArray( pnColumnData, nColumnCount );

    // Save column order
    g_Settings.SaveBinary( lpctszSectionName_i, _T( "Listvieworder" ), pnColumnData, nColumnCount );
 
    // Loop through and get columns width
    for( int nIndex = 0; nIndex < nColumnCount; ++nIndex )
    {
        pnColumnData[nIndex] = GetColumnWidth( nIndex );
    }// End for

    // Save column widths
    g_Settings.SaveBinary( lpctszSectionName_i, _T( "ListviewColumnWidths" ), pnColumnData, nColumnCount );

    // Delete allocated memory for column information
    delete [] pnColumnData;

    // Return success
    return true;
}


/** 
 * 
 * Restores saved state from settings.
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool CListCtrlEx::LoadState( LPCTSTR lpctszSectionName_i )
{
    // Column order variable
    int* pnColumnData = 0;

    // Column count variable
    UINT uColumnCount = 0;

    // Load column order from ini file
    g_Settings.LoadBinary( lpctszSectionName_i, _T( "Listvieworder" ), &pnColumnData, uColumnCount );

    // Verify retrieved data
    if( uColumnCount && pnColumnData )
    {
        // Set column order from stored settings
        SetColumnOrderArray( uColumnCount, pnColumnData );
        delete [] pnColumnData;
        pnColumnData = 0;
    }

    // Load column order from ini file
    g_Settings.LoadBinary( lpctszSectionName_i, _T( "ListviewColumnWidths" ), &pnColumnData, uColumnCount );
    
    // Verify data
    if( uColumnCount && pnColumnData )
    {
        // Set column order from stored settings
        for( UINT uIndex = 0; uIndex < uColumnCount; ++uIndex )
        {
            SetColumnWidth( uIndex, pnColumnData[uIndex] );
        }
        delete [] pnColumnData;
        pnColumnData = 0;
    }

    return true;
}// End LoadListControlState


/** 
 * 
 * Triggers sorting process.
 * 
 * @param       pNMHDR_i  - Header
 * @param       pResult_i - Result
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CListCtrlEx::OnColumnClickProcessModules( NMHDR* pNMHDR_i, LRESULT* pResult_i )
{
    // List view notification header
    NM_LISTVIEW* pNMListView = RCAST( NM_LISTVIEW*, pNMHDR_i );

    // Disable painting for now
    SetRedraw( FALSE );

    // Get sub item
    const int nSubItem = pNMListView->iSubItem;

    // Prepare sort data
	CListCtrl* pNonConstList = CCAST(CListCtrlEx*, this );
    DWORD dwCustom[ 3 ] = { nSubItem, m_Header.GetSortOrder(), RCAST( DWORD, pNonConstList ) };

    // Remove arrow from previous item
    m_Header.RemoveSortArrow();

    // Sort items
    ListView_SortItemsEx( GetSafeHwnd(), SortCB, RCAST( LPARAM, dwCustom ));

    // Set currently sorted item
    m_Header.SetSortItem( nSubItem );

    // Change sort arrow	
    m_Header.SetSortArrow();

    // Toggle sort order8
    m_Header.SetSortOrder( !m_Header.GetSortOrder() );

    // Return result
    *pResult_i = 0;

    SetRedraw( TRUE );
}


/** 
 * 
 * Sort callback function.
 * 
 * @param       lItemIndex1_i    - Item 1 index
 * @param       lItemIndex2_i    - Item 2 index
 * @param       lCustomParam_i   - Custom parameter passed in from main thread
 * @return      int              - Returns result from StrCmpLogicalW
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
int CALLBACK CListCtrlEx::SortCB( LPARAM lItemIndex1_i, 
                                  LPARAM lItemIndex2_i,
                                  LPARAM lCustomParam_i )
{
    const DWORD* pdwCustom = RCAST( DWORD*, lCustomParam_i );

    // Extract custom parameters
    const int nItemClicked = SCAST( int, pdwCustom[0] );
    const bool bSortAsc    = pdwCustom[1] == TRUE;
    const CListCtrlEx* pList = RCAST( CListCtrlEx*, pdwCustom[2] );

    // Get text of item 1
    const CString csItem1 = pList->GetItemText( lItemIndex1_i, nItemClicked );

    // Get text of item 2
    const CString csItem2 = pList->GetItemText( lItemIndex2_i, nItemClicked );

    // StrCmpLogicalW helps in comparing numbers intelligently, 10 is greater that 2, other 
    // wise string comparison will always return 2 is greater that 10
    return ( 
                bSortAsc ? ( pList->m_StrCmpLogicalWFuncPtr ? pList->m_StrCmpLogicalWFuncPtr : lstrcmp )( csItem1, csItem2 ) : 
                           ( pList->m_StrCmpLogicalWFuncPtr ? pList->m_StrCmpLogicalWFuncPtr : lstrcmp )( csItem2, csItem1 )
           );
}


/** 
 * 
 * Main window procedure
 * 
 * @param       uMessage_i - Message number
 * @param       wParam_i   - WPARAM
 * @param       lParam_i   - LPARAM
 * @return      LRESULT    - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
LRESULT CListCtrlEx::WindowProc( UINT uMessage_i, 
                                 WPARAM wParam_i, 
                                 LPARAM lParam_i )
{
    switch( uMessage_i )
    {
        case LVM_DELETEALLITEMS:
            m_Header.RemoveSortArrow();
            m_Header.SetSortOrder( true );
            break;
    }// End switch

    // Window procedure
    return CListCtrl::WindowProc( uMessage_i, wParam_i, lParam_i );
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
void CListCtrlEx::OnCopy()
{
    // Get selected item
    const int nSelectedItem = GetSelectionMark();
    if( -1 == nSelectedItem )
    {
        return;
    }

    // Hit test structure
    LVHITTESTINFO lvHitTestInfo = { 0 };

    // Get cursor position
    GetCursorPos( &lvHitTestInfo.pt );
    ScreenToClient( &lvHitTestInfo.pt );

    // Get list view client rectangle
    CRect crListClientRect;
    GetClientRect( &crListClientRect );

    // Get sub item on which the cursor is
    SubItemHitTest( &lvHitTestInfo );
    lvHitTestInfo.iItem = nSelectedItem;

    // Line break string to denote and item
    LPCTSTR lpctszLineBreak = _T( "\r\n" );

    // Final string to be copied
    CString csFinalCopyText;

    // Copy full item flag, if mouse point is outside list view rect
    // then copy entire item based on selection
    const bool bCopyFullItem = crListClientRect.PtInRect( lvHitTestInfo.pt ) == FALSE;

    // If we are copying a full item then first append the header control text first
    if( bCopyFullItem )
    {
        // First item will be header
        m_Header.ToString( csFinalCopyText );
        csFinalCopyText += lpctszLineBreak;
    }

    POSITION pstPos = GetFirstSelectedItemPosition();
    while( pstPos )
    {
        // Get selected item index
        const int nItem = GetNextSelectedItem( pstPos );

        // If no sub item under cursor then copy whole item
        if( bCopyFullItem )
        {
            CString csItem;
            ItemToString( nItem, csItem );

            // Append to copy text
            csFinalCopyText += csItem;
        }
        else
        {
            // Just copy subitem text
            csFinalCopyText += GetItemText( nItem, lvHitTestInfo.iSubItem );
        }// End if

        if( pstPos )
        {
            // Append a line break if this is not the last one
            csFinalCopyText += lpctszLineBreak;
        }// End if
    }// End while

    // Copy text to clipboard
    VERIFY( Utils::CopyTextToClipboard( GetSafeHwnd(), csFinalCopyText ));
}// End OnCopy


/** 
 * 
 * Selects all items in a list box.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CListCtrlEx::OnSelectAll()
{
    const int nItemCount = GetItemCount();
    for( int nIndex = 0; nIndex < nItemCount; ++nIndex )
    {
        SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED );
    }
}


/** 
 * 
 * Toggles selection state.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CListCtrlEx::OnToggleSelection()
{
    const int nItemCount = GetItemCount();
    for( int nIndex = 0; nIndex < nItemCount; ++nIndex )
    {
        // Toggle state
        const int nNewState = ( GetItemState( nIndex, LVIS_SELECTED ) == LVIS_SELECTED ? 0 : LVIS_SELECTED );

        // Set new state
        SetItemState( nIndex, nNewState, LVIS_SELECTED );
    }// End for
}

/** 
 * 
 * Converts an item to string form.
 * 
 * @param       nItem_i           - Item number
 * @param       csItem_o          - On return will hold item as string
 * @param       lpctszSeparator_i - Separator between items
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void CListCtrlEx::ItemToString( const int nItem_i, 
                                CString& csItem_o, 
                                LPCTSTR lpctszSeparator_i )
{
    const int nColumnCount = m_Header.GetItemCount();
    if( !nColumnCount )
    {
        return;
    }

    // Loop through and get text of item
    for( int nIndex = 0; nIndex < nColumnCount; ++nIndex )
    {
        csItem_o += GetItemText( nItem_i, nIndex ) + (( nIndex != nColumnCount - 1 ) ? lpctszSeparator_i : _T( "" ));
    }// End for
}// End ItemToString


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
void CListCtrlEx::OnKeyDown( UINT uChar_i, UINT uRepCnt_i, UINT uFlags_i ) 
{
    CListCtrl::OnKeyDown( uChar_i, uRepCnt_i, uFlags_i );

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
    else if(( uChar_i == 'a' || uChar_i == 'A' ) && Utils::IsCtrlDown() )
    {
        // Handle Ctrl + A
        OnSelectAll();
    }
    else if(( uChar_i == 't' || uChar_i == 'T' ) && Utils::IsCtrlDown() )
    {
        // Handle toggle selection
        OnToggleSelection();
    }// End if
}
