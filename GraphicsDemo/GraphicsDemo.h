#ifndef GRAPHCIS_DEMO_H_
#define GRAPHCIS_DEMO_H_

#include "SystemComponent.h"

class Object;

class GraphicsDemo : public SystemComponent
{
public:
    void OnStart() override;
    void Update(float dt) override;
    void Render() override;
    void Free() override;
    void OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParama, LPARAM lParam) override;

private:
    void Render(Object& object);
};

#endif // GRAPHCIS_DEMO_H_
