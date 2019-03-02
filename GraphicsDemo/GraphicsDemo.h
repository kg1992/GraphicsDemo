#ifndef GRAPHCIS_DEMO_H_
#define GRAPHCIS_DEMO_H_

#include "SystemComponent.h"
#include <glad.h>

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
    int m_clientWidth;
    int m_clientHeight;

    void Render(GLuint program, Object& object);
};

#endif // GRAPHCIS_DEMO_H_
