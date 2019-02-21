#ifndef WINTIME_H_INCLUDE_GUARD
#define WINTIME_H_INCLUDE_GUARD

//------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//------------------------------------------------------------


// Windows High Resolution Preformance Counter With Simpler Interface
// REAL will determine precision for "seconds".
template <typename REAL>
class WinTime
{
public:
      // output: clock measured by CPU.( Not in seconds )
      inline static LONGLONG Clock()
      {
            LARGE_INTEGER li;
            QueryPerformanceCounter(&li);
            return li.QuadPart;
      }

      // output: Frequency of Clocks in Hz( Clocks Per Seconds )
      //         Divide result from Clock to get seconds
      inline static REAL Hz()
      {
            return s_Hz;
      }

private:
      static const REAL s_Hz;

      static const REAL GetHz(void)
      {
            LARGE_INTEGER li;
            QueryPerformanceFrequency(&li);
            return static_cast<REAL>(li.QuadPart);
      }
};


template <typename REAL>
const REAL WinTime<REAL>::s_Hz = WinTime::GetHz();

#endif