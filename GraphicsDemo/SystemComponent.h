#ifndef SYSTEM_COMPONENT_H_
#define SYSTEM_COMPONENT_H_

#include <Windows.h>

class SystemComponent
{
public:
    virtual ~SystemComponent() {}
    virtual void Free() {}
    virtual void OnStart() {}
    virtual void Update(float time) {}
    virtual void OnEnd() {}
    virtual void OnBeforeRender() {}
    virtual void OnAfterRender() {}
    virtual void Render() {}
    virtual void OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {}
};

#endif
