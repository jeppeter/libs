// HeaderCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "processviewer.h"
#include "HeaderCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx

CHeaderCtrlEx::CHeaderCtrlEx() : m_bAscending( true ), m_nSortItem( 0 )
{
    // Load descending order bitmap
    VERIFY( m_DescBitmap.LoadBitmap( IDB_BITMAP_SORTDESC ));

    // Load ascending order bitmap
    VERIFY( m_AscBitmap.LoadBitmap( IDB_BITMAP_SORTASC ));

    // Create image list
    VERIFY( m_ImageList.Create( GetSystemMetrics( SM_CXSMICON ), 
                                GetSystemMetrics( SM_CYSMICON ), 
                                ILC_COLOR32 | ILC_MASK, 
                                0, 
                                2 ));

    VERIFY( m_ImageList.Add( &m_DescBitmap, RGB( 255, 255, 255 )) != -1 );
    VERIFY( m_ImageList.Add( &m_AscBitmap, RGB( 255, 255, 255 )) != -1 );
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrlEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHeaderCtrlEx::SetSortArrow()
{
    HDITEM sthdItem = { 0 };
    sthdItem.mask = HDI_IMAGE | HDI_FORMAT;
    sthdItem.fmt = HDF_IMAGE | HDF_STRING;
    sthdItem.iImage = GetSortOrder() ? 1 : 0;
    VERIFY( SetItem( GetSortItem(), &sthdItem ));
}// End SetSortArrow

void CHeaderCtrlEx::RemoveSortArrow()
{
    HDITEM sthdItem = { 0 };
    sthdItem.mask = HDI_FORMAT;
    sthdItem.fmt = HDF_STRING;
    VERIFY( SetItem( GetSortItem(), &sthdItem ));
}

void CHeaderCtrlEx::ToString( CString& csHeader_o, LPCTSTR lpctszSeparator_i )
{
    const int nCount = GetItemCount();

    // Text item
    TCHAR szText[50] = { 0 };
    for( int nIndex = 0; nIndex < nCount; ++nIndex )
    {
        HDITEM hdItem = { 0 };
        hdItem.mask = HDI_TEXT;
        hdItem.cchTextMax = 50;
        hdItem.pszText = szText;
        GetItem( nIndex, &hdItem );

        csHeader_o += hdItem.pszText;
        if( nIndex != nCount - 1 )
        {
            csHeader_o += lpctszSeparator_i;
        }
    }// End for
}// End ToString

LRESULT CHeaderCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == HDM_SETIMAGELIST )
    {
        if((( HIMAGELIST ) lParam ) != m_ImageList.GetSafeHandle() )
            return 0;
    }
	return CHeaderCtrl::WindowProc(message, wParam, lParam);
}
