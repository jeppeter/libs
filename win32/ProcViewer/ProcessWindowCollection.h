
#ifndef _PROCESS_WINDOW_COLLECTION_H_
#define _PROCESS_WINDOW_COLLECTION_H_

#include "window.h"

// Collection of windows for any process
typedef class ProcessWindowCollection
{
    public:

        ProcessWindowCollection( const DWORD dwProcessId_i );
        ~ProcessWindowCollection();

        //************************************
        // Method:    GetProcessWindowList
        // FullName:  ProcessWindowCollection::GetProcessWindowList
        // Access:    public 
        // Returns:   const WindowList&
        // Qualifier: const
        //************************************
        const WindowList& GetProcessWindowList() const { return m_wlProcessWindowList; }

        //************************************
        // Method:    GetProcessWindowList
        // FullName:  ProcessWindowCollection::GetProcessWindowList
        // Access:    public 
        // Returns:   WindowList&
        // Qualifier:
        //************************************
        WindowList& GetProcessWindowList(){ return m_wlProcessWindowList; }

        //************************************
        // Method:    GetProcessID
        // FullName:  ProcessWindowCollection::GetProcessID
        // Access:    public 
        // Returns:   DWORD
        // Qualifier: const
        //************************************
        DWORD GetProcessID() const { return m_dwProcessId; }

        // Return count of main process windows
        //************************************
        // Method:    GetProcessWindowCount
        // FullName:  ProcessWindowCollection::GetProcessWindowCount
        // Access:    public 
        // Returns:   DWORD
        // Qualifier: const
        //************************************
        INT_PTR GetProcessWindowCount() const { return m_wlProcessWindowList.GetCount(); }

        // Clears window list
        void Clear();

        //************************************
        // Method:    GetWindowMap
        // FullName:  ProcessWindowCollection::GetWindowMap
        // Access:    public 
        // Returns:   WindowMap&
        // Qualifier:
        //************************************
        WindowMap& GetWindowMap()
        {
            return m_wmWindowMap;
        }

        
        //************************************
        // Method:    GetWindowMap
        // FullName:  ProcessWindowCollection::GetWindowMap
        // Access:    public 
        // Returns:   const WindowMap&
        // Qualifier: const
        //************************************
        const WindowMap& GetWindowMap() const
        {
            return m_wmWindowMap;
        }

        //************************************
        // Method:    GetTotalWindowCount
        // FullName:  ProcessWindowCollection::GetTotalWindowCount
        // Access:    public 
        // Returns:   int
        // Qualifier: const
        //************************************
        int GetTotalWindowCount() const { return GetWindowMap().GetCount(); }

        // Load all windows for this process
        bool LoadProcessWindow( HWND hNewWindow_i );

    private:

        static BOOL CALLBACK EnumerateChildWindowsCB( HWND hWnd_i, LPARAM lParam_i );

        // Disable assignment and copy construction
        ProcessWindowCollection& operator= ( const ProcessWindowCollection& );
        ProcessWindowCollection( const ProcessWindowCollection& );

        // Process id
        const DWORD m_dwProcessId;

        // List of process main windows
        WindowList m_wlProcessWindowList;

        // Map for child windows, for fast lookup
        WindowMap m_wmWindowMap;
}*PProcessWindowCollection; // End ProcessWindowCollection

#endif // _PROCESS_WINDOW_COLLECTION_H_