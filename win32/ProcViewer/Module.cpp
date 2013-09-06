#include "stdafx.h"
#include "Module.h"



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
Module::Module() : m_hModuleHandle( 0 )
{
    Clear();
}


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
Module::~Module()
{
    Clear();
}


/** 
 * 
 * Clears internal data members.
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Module::Clear()
{
    m_hModuleHandle && CloseHandle( m_hModuleHandle );
    m_hModuleHandle = 0;

    ::ZeroMemory( &m_stModuleEntry32, sizeof( m_stModuleEntry32 ));
    m_stModuleEntry32.dwSize = sizeof( m_stModuleEntry32 );
}