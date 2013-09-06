#include "stdafx.h"
#include "ProcessCollection.h"
#include "Tlhelp32.h"


/** 
 * 
 * Default constructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ProcessCollection::ProcessCollection() 
    :   m_dwProcessCount( 0 )
{
    // Set size of array
    m_mapProcesses.InitHashTable( 53, FALSE );
}


/** 
 * 
 * Destructor
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ProcessCollection::~ProcessCollection()
{
    Clear();
}


/** 
 * 
 * Enumerates and maintains a list of processes running in a system.
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool ProcessCollection::Enumerate()
{
    // Clear previous information
    Clear();

    // Process snapshot
    HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        ASSERT( FALSE );
        return false;
    }

    // Auto close handle
    Utils::AutoHandleMgr ahmHandleMgr( hProcessSnap );

    // Process entry structure
    PROCESSENTRY32 stpePrcEntry32 = { 0 };
    stpePrcEntry32.dwSize = sizeof( stpePrcEntry32 );

    // Get first process
    if( !Process32First( hProcessSnap, &stpePrcEntry32 ))
    {
        ASSERT( FALSE );
        return false;
    }

    // Get process ids of all running processes
    DWORD dwRealProcessCount = 0;

    // Loop through and get each process
    do 
    {
        // Increment process count
        ++dwRealProcessCount;

        // Allocate new process
        Process* pNewProcess = new Process;
        if( !pNewProcess )
        {
            return false;
        }

        // Set process information
        pNewProcess->SetProcessEntry32( stpePrcEntry32 );

        // Process information
        Process* pParentProcess = 0;
        if( m_mapProcesses.Lookup( stpePrcEntry32.th32ParentProcessID, pParentProcess ) && pParentProcess )
        {
            pParentProcess->AddKid( pNewProcess );
        }
        else
        {
            // Since this process doesn't have a parent we will add this to super process list
            m_lstSuperProcesses.AddTail( pNewProcess );
        }// End if

        // Insert this process to map
        m_mapProcesses[stpePrcEntry32.th32ProcessID] = pNewProcess;
    }while( Process32Next( hProcessSnap, &stpePrcEntry32 ));

    // Set count of loaded processes
    SetProcessCount( dwRealProcessCount );

    // Return success
    return true;
}


/** 
 * 
 * Clears previous information from array
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void ProcessCollection::Clear()
{
    // Get count of all processes loaded in a system
    POSITION pstPos = m_mapProcesses.GetStartPosition();
    while( pstPos )
    {
        DWORD dwProcessID = 0;
        Process* pProcess = 0;

        // Get next item from map
        m_mapProcesses.GetNextAssoc( pstPos, dwProcessID, pProcess );

        // Delete this entry
        delete pProcess;
        pProcess = 0;
    }// End while

    // Remove all entries from map
    m_mapProcesses.RemoveAll();
}