#ifndef GRAPHCIS_DEMO_H_
#define GRAPHCIS_DEMO_H_

#include <glad.h>
#include <vector>
#include "SystemComponent.h"
#include "Lights.h"
#include "Camera.h"

class Object;
class ShaderProgram;
class Material;

class GraphicsDemo : public SystemComponent
{
public:
    GraphicsDemo();

    void OnStart() override;
    void Update(float dt) override;
    void Render() override; 
    void Free() override;
    void OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParama, LPARAM lParam) override;

private:
    int m_clientWidth;
    int m_clientHeight;
    int m_lastMouseX;
    int m_lastMouseY;
    bool m_mousePosRecordStarted;
    std::vector<std::shared_ptr<Object>> m_objects;
    Camera m_camera;
    std::vector<PointLight> m_pointLights;

    void RenderObject(ShaderProgram& program, Object& object);
    void AddGround();
    void DrawPointLights();
    void DrawScene();
    void DrawPeekViewports();
    void DrawObjectCenter();
    void PrepareLights();
    void SendPointLight(ShaderProgram& program, int index, PointLight& light);
    void SendLights(ShaderProgram& program, int count);
    void SendMaterial(ShaderProgram& program);
    void SendMatrices(ShaderProgram& program, Object& object);
};

#endif
