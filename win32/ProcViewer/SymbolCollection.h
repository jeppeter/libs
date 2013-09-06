/**
 *
 * SymbolCollection.h - Interface declaration for SymbolCollection
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-20
 */

#ifndef _MODULE_SYMBOL_COLLECTION_H_
#define _MODULE_SYMBOL_COLLECTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dbghelp.h>
#include <afxtempl.h>

class SymbolData
{
public:

    SymbolData() : 
		m_SymbolInfo( 0 )
    {}

	~SymbolData()
	{
		FreeSymbolInfo();
	}

	void SetSymbolData( const SYMBOL_INFO& SymbolInfo )
	{
		FreeSymbolInfo();
		const int AllocationSize = sizeof( SymbolInfo ) + SymbolInfo.MaxNameLen + 1;
		m_SymbolInfo = (PSYMBOL_INFO)new BYTE[AllocationSize];
		memcpy( m_SymbolInfo, &SymbolInfo, AllocationSize );
	}

	void FreeSymbolInfo()
	{
		if( m_SymbolInfo )
		{
			delete m_SymbolInfo;
			m_SymbolInfo = 0;
		}
	}

	bool GetSymbolType( CString& csType ) const;
	bool IsValuePresent() const
	{
		return m_SymbolInfo && Utils::IsValidMask( m_SymbolInfo->Flags, SYMFLAG_VALUEPRESENT );
	}
  
	PSYMBOL_INFO m_SymbolInfo;
    CString csSymbolName;
    ULONG ulSymbolAddress;
    ULONG ulSymbolSize;
};

typedef SymbolData* PSymbolData;


// Typedef for symbol list
typedef CList<PSymbolData, PSymbolData&> SymbolList;

#define MAKE_FLAG_MAPPER_ITEM( Style ) { Style, _T( #Style )}
struct FlagDescMapper
{
	UINT Flag;
	LPCTSTR lpctszFlagDesc;
};

extern FlagDescMapper g_FlagDescMapper[];

/**
 *
 * SymbolCollection - Maintains a collection of symbols in a given module.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-20
 */
class SymbolCollection
{
    public:
        
	    SymbolCollection();
	    virtual ~SymbolCollection();

        // Adds a symbol to list
        void AddSymbol( PSymbolData pstSymData_i );

        // Returns reference to symbol list
        SymbolList& GetSymbolList()
        {
            return m_slSymbolList;
        }

        // Const version of above function
        const SymbolList& GetSymbolList()const
        {
            return m_slSymbolList;
        }

        // Returns count of symbols
        DWORD GetSymbolCount() const
        {
            return m_slSymbolList.GetCount();
        }

        // Clears symbol list
        void Clear();

        // Triggers traversal process
        bool StartTraversal();
        bool GetNext( PSymbolData& pstSymData_o );

    private:
        
        // Symbol position
        POSITION m_pstSymbolPos;

        // Symbol list
        SymbolList m_slSymbolList;
        
};// End SymbolCollection

#endif // _MODULE_SYMBOL_COLLECTION_H_