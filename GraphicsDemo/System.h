#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Singleton.h"
#include "Clock.h"

class SystemComponent;

class System : public Singleton<System>
{
public:
    System();

    void Run();

    float CurrentTime();

    void AddComponent(SystemComponent* pComponent);

    template< typename T>
    void GetComponent(T*& pOut)
    {
        for (std::shared_ptr<SystemComponent> pComponent : m_components)
        {
            if (typeid(*pComponent) == typeid(T))
                pOut = dynamic_cast<T*>(pComponent.get());
        }
    }

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
