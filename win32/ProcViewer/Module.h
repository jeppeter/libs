#ifndef _MODULE_H_
#define _MODULE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileVersionInfo.h"
#include <TLHELP32.H>

class Module
{
    public:

        Module();
        virtual ~Module();

        // Set module entry
        void SetModuleEntry32( const MODULEENTRY32& stModuleEntry32_i )
        {
            m_stModuleEntry32 = stModuleEntry32_i;
        }

        // Return reference to module entry
        const MODULEENTRY32& GetGetModuleEntry32() const
        {
            return m_stModuleEntry32;
        }

        // Return module file handle
        const HMODULE& GetModuleHandle() const { return m_hModuleHandle; }

        // Clear internal members
        void Clear();

    private:

        // Module handle
        HMODULE m_hModuleHandle;

        // Entry for a module
        MODULEENTRY32 m_stModuleEntry32;
        FileVersionInfo m_fviVersion;

};// End class Module

#endif // _MODULE_H_
