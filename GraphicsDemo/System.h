#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <Windows.h>
#include "Singleton.h"
#include "Clock.h"

class System : public Singleton<System>
{
public:
    System();

    bool Init();

    void Loop();

    void Free();

private:
    HWND m_hwnd;
    HDC m_hdc;
    HGLRC m_hglrc;
    bool m_quit;
    Clock m_clock;
    float m_lastFrameEndTime;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetQuitFlag();

    void Render();
};

#endif
