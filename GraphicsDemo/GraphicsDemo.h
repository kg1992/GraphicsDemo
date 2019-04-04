/*
    GraphicsDemo.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    GraphicsDemo class definition.
*/
#ifndef GRAPHCIS_DEMO_H_
#define GRAPHCIS_DEMO_H_

#include "SystemComponent.h"
#include "Lights.h"
#include "Camera.h"
#include "SceneRenderer.h"
#include "PeekViewportRenderer.h"
#include "GizmoRenderer.h"
#include "SkyboxRenderer.h"

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
    std::shared_ptr<Scene> m_pScene;
    SceneRenderer m_sceneRenderer;
    PeekViewportRenderer m_peekViewportRenderer;
    GizmoRenderer m_gizmoRenderer;
    SkyboxRenderer m_skyboxRenderer;

    Camera& GetCamera();

    bool SaveScene(const std::string& sceneName);
    bool LoadScene(const std::string& sceneName);
    void RenderScreen();
};

#endif
