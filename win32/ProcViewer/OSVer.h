#ifndef _OS_VER_H_
#define _OS_VER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * OSVer - Extracts OSVer Version.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-08
 */
class OSVer  
{
    public:

        virtual ~OSVer();
        static OSVer& Instance()
        {
            static OSVer __os;
            return __os;
        }

        // Returns service pack string
        LPCTSTR GetServicePack() const;

        // Returns true if OSver is 2003
        bool Is2003() const;

        // Returns true if OSVer is XP
        bool IsXP() const;

        // Returns true if OSVer is 2000
        bool Is2000() const;

        // Returns true if OSVer is windows 95
        bool IsWin95() const;

        // Returns true if OSVer is windows 98
        bool IsWin98() const;

        // Returns true if OSVer is windows ME
        bool IsWinME() const;

        // Return OS Ver constant reference
        const OSVERSIONINFO& GetOSVer() const;

    private:

        OSVer();

        // OSVer Version
        OSVERSIONINFO m_stOsVer;
};

#endif // _OS_VER_H_