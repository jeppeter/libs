// ProcessCollection.h: interface for the ProcessCollection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PROCESS_COLLECTION_H_
#define _PROCESS_COLLECTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"
#include "afxtempl.h"

#define MAX_PROCESSES 1024

/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * ProcessCollection - Collection of processes in a system
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-15
 */
class ProcessCollection  
{
    public:

	    ProcessCollection();
	    virtual ~ProcessCollection();

        // Enumerate
        bool Enumerate();

        // Set's process count
        void SetProcessCount( const DWORD dwProcessCount_i )
        {
            m_dwProcessCount = dwProcessCount_i;
        }

        // Returns process count
        DWORD GetProcessCount()
        {
            return m_dwProcessCount;
        }

        // Clear previous process information
        void Clear();

    private:

        DWORD m_dwProcessCount;
        
        CMap<DWORD, const DWORD, Process*, Process*&> m_mapProcesses;
        CList<Process*, Process*&> m_lstSuperProcesses;
};

#endif // _PROCESS_COLLECTION_H_