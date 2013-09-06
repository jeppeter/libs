#ifndef _WINDOW_COLLECTION_H_
#define _WINDOW_COLLECTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"
#include "TreeCtrlEx.h"
#include "ProcessWindowCollection.h"

/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * WindowCollection - Enumerates all parent windows and maintains a collection of such 
 *                    windows.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-01
 */
class WindowCollection  
{
    public:

	    WindowCollection();
	    virtual ~WindowCollection();

        // Main enumerator
        bool Enumerate();

        // Clear map
        void Clear();

        // Returns window handle for process id
        const PProcessWindowCollection GetProcessWindowCollection( const DWORD dwProcessID_i ) const;

    private:
        
        // Window enumeration function
        static BOOL CALLBACK EnumerateWindowsCB( HWND hWnd_i, LPARAM lParam_i );
        CMap<DWORD, DWORD, PProcessWindowCollection, PProcessWindowCollection&> m_WindowDataMap;
};

#endif // _WINDOW_COLLECTION_H_