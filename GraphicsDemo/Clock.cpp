#include "Clock.h"
#include <Windows.h>

namespace
{
    Clock::tick_type GetApplicationTick()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return static_cast<Clock::tick_type>(li.QuadPart);
    }
    Clock::sec_type GetApplicationFrequency()
    {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        return static_cast<Clock::sec_type>(li.QuadPart);
    }
}

Clock::Clock()
    : m_origin(GetApplicationTick())
{
}
Clock::Clock(tick_type tick)
    : m_origin(GetApplicationTick() - tick)
{
}
const Clock::tick_type Clock::operator-(const Clock& rhs) const
{
    return GetOrigin() - rhs.GetOrigin();
}
Clock::tick_type Clock::GetOrigin() const
{
    return m_origin;
}
Clock::tick_type Clock::GetTick() const
{
    return GetApplicationTick() - GetOrigin();
}
Clock::sec_type Clock::GetSec() const
{
    return (GetApplicationTick() - GetOrigin()) / GetApplicationFrequency();
}
Clock::sec_type Clock::Reset()
{
    auto sec = GetSec();
    m_origin = GetApplicationTick();
    return sec;
}