#ifndef CLOCK_H_
#define CLOCK_H_

class Clock
{
public:
    // tick type. Unit may vary depending on system.
    typedef long long   tick_type;
    // sec type. Unit is second.
    typedef float       sec_type;
private:
    // Where clock begun its timer
    tick_type m_origin;
public:
    // Construct Clock so that curren second be 0
    Clock();
    // Construct Clock so that 'tick' is current 'tick' of the clock.
    explicit Clock(tick_type tick);
    // Get difference between two Clocks to return tick
    const tick_type operator-(const Clock& rhs) const;
    tick_type GetOrigin() const;
    tick_type GetTick() const;
    sec_type GetSec() const;
    sec_type Reset();
}; // class Clock

#endif
