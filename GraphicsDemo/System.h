#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <Windows.h>
#include "Singleton.h"

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

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetQuitFlag();
};

#endif
