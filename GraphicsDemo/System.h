#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <Windows.h>
#include <vector>
#include "Singleton.h"
#include "Clock.h"
#include <glm/glm.hpp>

class SystemComponent;

class System : public Singleton<System>
{
public:
    System();

    void Run();

    float CurrentTime();

    void AddComponent(SystemComponent* pComponent);

private:
    HWND m_hwnd;
    HDC m_hdc;
    HGLRC m_hglrc;
    bool m_quit;
    Clock m_clock;
    float m_lastFrameEndTime;
    std::vector<std::shared_ptr<SystemComponent>> m_components;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetQuitFlag();

    void Render();

    bool Init();

    void Loop();

    void Free();
};

#endif
