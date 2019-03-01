#ifndef GRAPHCIS_DEMO_H_
#define GRAPHCIS_DEMO_H_

#include "SystemComponent.h"

class GraphicsDemo : public SystemComponent
{
public:
    void OnStart() override;
    void Render() override;
    void Free() override;
    void OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParama, LPARAM lParam) override;
};

#endif // GRAPHCIS_DEMO_H_
