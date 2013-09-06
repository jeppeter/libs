/**
 *
 * SymbolCollection.cpp - Implementation file for SymbolCollection.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-20
 */


#include "stdafx.h"
#include "SymbolCollection.h"

// Flat description
extern FlagDescMapper g_FlagDescMapper[] = 
											{
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_CONSTANT ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_EXPORT ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_FORWARDER ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_FRAMEREL ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_FUNCTION ),
												#if 0
													MAKE_FLAG_MAPPER_ITEM( SYMFLAG_ILREL ),
													MAKE_FLAG_MAPPER_ITEM( SYMFLAG_METADATA ),
												#endif
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_PARAMETER ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_REGISTER ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_REGREL ),
												#if 0
													MAKE_FLAG_MAPPER_ITEM( SYMFLAG_SLOT ),
												#endif
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_THUNK ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_TLSREL ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_VALUEPRESENT ),
												MAKE_FLAG_MAPPER_ITEM( SYMFLAG_VIRTUAL ),
												{ 0, 0 }
											};

// Get symbol type
bool SymbolData::GetSymbolType( CString& csType ) const
{
	if( !m_SymbolInfo )
	{
		return false;
	}

	// Separator between symbol types
	LPCTSTR lpctszSeparator = _T( " | " );

	for( int nIndex = 0; g_FlagDescMapper[nIndex].lpctszFlagDesc; ++nIndex )
	{
		if( Utils::IsValidMask( m_SymbolInfo->Flags, g_FlagDescMapper[nIndex].Flag ))
		{
			csType += g_FlagDescMapper[nIndex].lpctszFlagDesc;
			csType += lpctszSeparator;
		}// End if
	}// End for

	// Remove last separator
	csType.TrimRight( lpctszSeparator );
	return true;
}// GetSymbolType


/** 
 * 
 * Default constructor.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
SymbolCollection::SymbolCollection() : m_pstSymbolPos( 0 )
{}


/** 
 * 
 * Destructor.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
SymbolCollection::~SymbolCollection()
{
    Clear();
}


/** 
 * 
 * Add given symbol to list.
 * 
 * @param       pstSymData_i - Symbol data.
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SymbolCollection::AddSymbol( PSymbolData pstSymData_i )
{
    // Add to list
    m_slSymbolList.AddTail( pstSymData_i );
}


/** 
 * 
 * Clears internal list.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void SymbolCollection::Clear()
{
    POSITION pstPos = m_slSymbolList.GetHeadPosition();
    while( pstPos )
    {
        // Get next symbol
        PSymbolData& pstSymData = m_slSymbolList.GetNext( pstPos );

        // Free symbol data
        delete pstSymData;
        pstSymData = 0;
    }// End while

    // Clear list
    m_slSymbolList.RemoveAll();
    m_pstSymbolPos = 0;
}// End Clear


/** 
 * 
 * Starts traversal process
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool SymbolCollection::StartTraversal()
{
    m_pstSymbolPos = m_slSymbolList.GetHeadPosition();
    return m_pstSymbolPos != 0;
}


/** 
 * 
 * Returns next symbol data from list
 * 
 * @param       pstSymData_o - Return symbol data
 * @return      bool         - Returns true if there is more
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool SymbolCollection::GetNext( PSymbolData& pstSymData_o )
{
    bool bFound = m_pstSymbolPos != 0;
    if( m_pstSymbolPos )
    {
        pstSymData_o = m_slSymbolList.GetNext( m_pstSymbolPos );
    }
	else
	{
		pstSymData_o = 0;
	}

    // Returns execution status
    return bFound;
}