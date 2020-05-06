#pragma once

#ifndef RM_PLATFORM_WINDOWS
#include <chrono>

typedef std::chrono::high_resolution_clock hr_clock;
typedef std::chrono::nanoseconds clock_freq;
#endif

class Clock
{
public:
    Clock();
    ~Clock();

    // Records current time in start variable
    void Start(void);

    // Records current time in stop variable
    void Stop(void);

    // Time since last Start call
#ifdef RM_PLATFORM_WINDOWS
    float Elapsed(void);
#else
    long long Elapsed(void);
#endif

    // Time between last Start and Stop calls
#ifdef RM_PLATFORM_WINDOWS
    float Difference(void);
#else
    long long Difference(void);
#endif

    // Get the current clock count
#ifdef RM_PLATFORM_WINDOWS
    LONGLONG Current(void);
#else
    long long Current(void);
#endif

#ifdef RM_PLATFORM_WINDOWS
private:
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_start, m_stop, m_current;

    void Query(LARGE_INTEGER& query);
#else
private:
    hr_clock::time_point m_start;
    hr_clock::time_point m_stop;
    hr_clock::time_point m_current;
#endif
};