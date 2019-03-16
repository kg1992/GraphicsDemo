/*
    GraphicsDemo.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    class GraphicsDemo implementation.
*/
#include "Common.h"
#include "GraphicsDemo.h"
#include "Errors.h"
#include "System.h"
#include "Object.h"
#include "stb_image.h"
#include "ShaderPrograms.h"
#include "FontRenderer.h"
#include "Material.h"
#include "FbxLoader.h"
#include "KnightPunchingScene.h"
#include "Serialization.h"

namespace
{
    const int VK_0 = 0x30;
    const int VK_1 = 0x31;
    const int VK_2 = 0x32;
    const int VK_3 = 0x33;
    const int VK_4 = 0x34;
    const int VK_5 = 0x35;
    const int VK_6 = 0x36;
    const int VK_7 = 0x37;
    const int VK_8 = 0x38;
    const int VK_9 = 0x39;
    const int VK_A = 0x41;
    const int VK_B = 0x42;
    const int VK_C = 0x43;
    const int VK_D = 0x44;
    const int VK_E = 0x45;
    const int VK_F = 0x46;
    const int VK_G = 0x47;
    const int VK_H = 0x48;
    const int VK_I = 0x49;
    const int VK_J = 0x4A;
    const int VK_K = 0x4B;
    const int VK_L = 0x4C;
    const int VK_M = 0x4D;
    const int VK_N = 0x4E;
    const int VK_O = 0x4F;
    const int VK_P = 0x50;
    const int VK_Q = 0x51;
    const int VK_R = 0x52;
    const int VK_S = 0x53;
    const int VK_T = 0x54;
    const int VK_U = 0x55;
    const int VK_V = 0x56;
    const int VK_W = 0x57;
    const int VK_X = 0x58;
    const int VK_Y = 0x59;
    const int VK_Z = 0x5A;

    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
    FontRenderer s_fontRenderer;
}

GraphicsDemo::GraphicsDemo()
    : m_lastMouseX(0)
    , m_lastMouseY(0)
    , m_mousePosRecordStarted(false)
    , m_pScene()
{
}

void GraphicsDemo::OnStart()
{
    // Load, compile, link shader programs.
    ShaderPrograms::Init();

    const std::string OutPath = "Resources/Scene/";
    const std::string OutMyScene = OutPath + "KnightPunchingScene.dat";
    std::shared_ptr<Scene> pScene(new Scene);
    std::ifstream ifs(OutMyScene, std::ios_base::in | std::ios_base::binary);
    pScene->Deserialize(ifs);
    ifs.close();

    m_pScene = pScene;
    
    m_pScene->Init();
}

void GraphicsDemo::Update(float dt)
{
    m_pScene->Update();
}

void GraphicsDemo::Render()
{
    glViewport(0, 0, m_clientWidth, m_clientHeight);
    GET_AND_HANDLE_GL_ERROR();

    glClearColor(0, 0.8f, 0.7f, 1.0f);
    GET_AND_HANDLE_GL_ERROR();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    m_sceneRenderer.Render(m_pScene);

    m_gizmoRenderer.Render(m_pScene);

    // s_fontRenderer.RenderText(U"Hi", 0, 100);

    glClear(GL_DEPTH_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    m_peekViewportRenderer.Render(m_pScene);
}

void GraphicsDemo::Free()
{
    m_pScene->Free();
}

void GraphicsDemo::OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        m_clientWidth = width;
        m_clientHeight = height;

        m_peekViewportRenderer.SetClientRect(width, height);

        if( m_pScene )
            GetCamera().SetFrustum(glm::pi<float>() * 0.25f, width / 0.5f, height / 0.5f,
                0.1f, 1000.0f);
    }

    case WM_MOUSEMOVE:
    {
        // ref : https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-mousemove
        // ref : https://docs.microsoft.com/en-us/windows/desktop/api/windowsx/nf-windowsx-get_x_lparam
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        int dx, dy;
        if (!m_mousePosRecordStarted)
        {
            dx = dy = 0;
            m_mousePosRecordStarted = true;
        }
        else
        {
            dx = x - m_lastMouseX;
            dy = y - m_lastMouseY;
        }

        if (wParam & MK_CONTROL && wParam & MK_MBUTTON)
        {
            glm::vec3 disp = GetCamera().GetCenter() - GetCamera().GetPosition();
            if (dy > 0) {
                for (int i = 0; i < dy; ++i) {
                    GetCamera().SetPosition(GetCamera().GetPosition() - disp * 0.025f);
                }
            }
            else if (dy < 0)
            {
                for (int i = 0; i > dy; --i) {
                    GetCamera().SetPosition(GetCamera().GetPosition() + disp * 0.05f);
                }

            }
        }

        else if (wParam & MK_SHIFT && wParam & MK_MBUTTON)
        {
            glm::mat4x4 invEye = glm::inverse(GetCamera().EyeMatrix());
            glm::vec4 ex = invEye[0];
            glm::vec4 ey = invEye[1];

            GetCamera().MoveBy(glm::vec3(ex) * (float)dx);
            GetCamera().MoveBy(glm::vec3(ey) * -(float)dy);
        }

        else if (wParam & MK_MBUTTON)
        {
            const float Speed = 0.02f;
            // relative displacement from center to eye
            glm::vec3 r = GetCamera().GetPosition() - GetCamera().GetCenter();

            // revolve around center horizontally
            r = glm::rotate(r, -dx * Speed, GetCamera().GetUp());

            // revolve around center vertically
            glm::vec3 right = glm::normalize(glm::cross(r, GetCamera().GetUp()));
            r = glm::rotate(r, dy * Speed, right);
            GetCamera().SetPosition(GetCamera().GetCenter() + r);
        }

        m_lastMouseX = x;
        m_lastMouseY = y;
        break;
    }

    case WM_MOUSEWHEEL:
    {
        // ref : https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-mousewheel
        short wheel = HIWORD(wParam);

        glm::vec3 disp = GetCamera().GetCenter() - GetCamera().GetPosition();

        GetCamera().SetPosition(GetCamera().GetPosition() + disp * 0.025f * wheel / WHEEL_DELTA);
        break;
    }

    case WM_KEYUP:
        if (wParam == VK_R)
        {
            GetCamera().LookAt(glm::vec3(50, 50, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }
        break;


    case WM_LBUTTONDOWN:case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:case WM_MBUTTONUP:
    case WM_KILLFOCUS: case WM_SETFOCUS:
    case WM_KEYDOWN: break;
    }
}

Camera& GraphicsDemo::GetCamera()
{
    return m_pScene->GetCamera();
}