// ***************************************************************
//  Performance   version:  1.0   ·  date: 07/21/2007
//  -------------------------------------------------------------
//  Author Nibu babu thomas
// ***************************************************************

#ifndef _PERFORMANCE_H_
#define _PERFORMANCE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////
//Usage 
// Performance PerfObj;
// PerfObj.Start();
// CallPerformanceTestingOperationHere();
// PerfObj.Stop(); Returns time difference
///////////////////////

class Performance
{
    public:

	    Performance()
        {}

	    virtual ~Performance()
        {}

        void Init()
        {
            ZeroMemory( &m_liStartTime, sizeof( m_liStartTime ));
            ZeroMemory( &m_liEndTime, sizeof( m_liEndTime ));
            ZeroMemory( &m_liFrequency, sizeof( m_liFrequency ));

            VERIFY( QueryPerformanceFrequency( &m_liFrequency ));
        }

        void Start()
        {
            Init();
            VERIFY( QueryPerformanceCounter( &m_liStartTime ));
        }

        double Stop()
        {
            VERIFY( QueryPerformanceCounter( &m_liEndTime ));
            
            LARGE_INTEGER liDifference = { 0 };
            liDifference.QuadPart = ( m_liEndTime.QuadPart - m_liStartTime.QuadPart );
            return ( SCAST( double, liDifference.QuadPart )
                                        /
                     SCAST( double, m_liFrequency.QuadPart ));
        }

    private:

        LARGE_INTEGER m_liStartTime;
        LARGE_INTEGER m_liEndTime;
        LARGE_INTEGER m_liFrequency;
};

#endif // _PERFORMANCE_H