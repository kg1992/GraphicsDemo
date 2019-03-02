#include "SystemComponent.h"
#include "GraphicsDemo.h"
#include "Errors.h"
#include <iostream>
#include <glad.h>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Windowsx.h>
#include <WinUser.h>
#include "System.h"
#include "Object.h"
#include "Camera.h"
#include "stb_image.h"
#include "ShaderPrograms.h"

void LoadFbxSdkObject(const char* const filename, std::vector<std::shared_ptr<Object>>& objectStack);
std::shared_ptr<Mesh> GeneratePlane();

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 color;
};

namespace
{
    std::vector<std::shared_ptr<Object>> s_objectStack;

    std::vector<PointLight> pointLights;

    Object object;
    Camera camera;

    int s_lastMouseX = 0, s_lastMouseY = 0;
    bool s_mousePosRecordStarted = false;

    
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
}

void GraphicsDemo::OnStart()
{
    ShaderPrograms::Init();

    camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));

    LoadFbxSdkObject("./Resources/56-fbx/fbx/Dragon 2.5_fbx.fbx", s_objectStack);
    s_objectStack.back()->SetRotation(glm::angleAxis(glm::pi<float>()*-0.5f, glm::vec3(1, 0, 0)));

    PointLight pointLight;
    pointLight.position = glm::vec3(0, 10, 0);;
    pointLights.push_back(pointLight);
        
    std::shared_ptr<Object> pPlaneObject(new Object);
    pPlaneObject->SetMesh(GeneratePlane());
    pPlaneObject->SetScale(glm::vec3(100, 1, 100));
    s_objectStack.push_back(pPlaneObject);
}

void GraphicsDemo::Update(float dt)
{
    const float M_PI = 3.14159f;
    const float currentTime = System::Instance()->CurrentTime();
    const float f = (float)currentTime * (float)M_PI * 0.1f;

    glm::mat4 mvMatrix = glm::identity<glm::mat4>();

    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    mvMatrix = glm::translate(mvMatrix, glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) *0.5f, sinf(1.3f*f)));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 45.0f / M_PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 81.0f / M_PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));

    object.SetTransformMatrix(mvMatrix);
}

void GraphicsDemo::Render()
{
    glViewport(0, 0, m_clientWidth, m_clientHeight);
    glClearColor(0, 0.8f, 0.7f, 1.0f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Render(object);
    for (auto& pObject : s_objectStack)
    {
        Render(ShaderPrograms::s_basic, *pObject);
    }

    int peekViewWidth = m_clientWidth / 8;
    int peekViewHeight = m_clientHeight / 8;
    GLuint programs[] = { ShaderPrograms::s_position, ShaderPrograms::s_uv, ShaderPrograms::s_normal };

    glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < 3; ++i)
    {
        glViewport(peekViewWidth * i, 0, peekViewWidth, peekViewHeight);

        for (auto& pObject : s_objectStack)
        {
            Render(programs[i], *pObject);
        }
    }
}

void GraphicsDemo::Free()
{
    object.Free();
}

void GraphicsDemo::OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        m_clientWidth = LOWORD(lParam);
        m_clientHeight = HIWORD(lParam);
        camera.SetFrustum(glm::pi<float>() * 0.25f, m_clientWidth / 0.5f, m_clientHeight / 0.5f,
            0.1f, 1000.0f);

    case WM_MOUSEMOVE:
    {
        // ref : https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-mousemove
        // ref : https://docs.microsoft.com/en-us/windows/desktop/api/windowsx/nf-windowsx-get_x_lparam
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        int dx, dy;
        if (!s_mousePosRecordStarted)
        {
            dx = dy = 0;
            s_mousePosRecordStarted = true;
        }
        else
        {
            dx = x - s_lastMouseX;
            dy = y - s_lastMouseY;
        }

        if (wParam & MK_CONTROL && wParam & MK_MBUTTON)
        {
            glm::vec3 disp = camera.GetCenter() - camera.GetPosition();
            if (dy > 0) {
                for (int i = 0; i < dy; ++i) {
                    camera.SetPosition(camera.GetPosition() - disp * 0.025f);
                }
            }
            else if (dy < 0)
            {
                for (int i = 0; i > dy; --i) {
                    camera.SetPosition(camera.GetPosition() + disp * 0.05f);
                }

            }
        }

        else if (wParam & MK_SHIFT && wParam & MK_MBUTTON)
        {
            glm::mat4x4 invEye = glm::inverse(camera.EyeMatrix());
            glm::vec4 ex = invEye[0];
            glm::vec4 ey = invEye[1];

            camera.MoveBy(ex.xyz * (float)dx);
            camera.MoveBy(ey.xyz * -(float)dy);
        }

        else if (wParam & MK_MBUTTON)
        {
            // relative displacement from center to eye
            glm::vec3 r = camera.GetPosition() - camera.GetCenter();

            // revolve around center horizontally
            r = glm::rotate(r, -dx * 0.05f, camera.GetUp());

            // revolve around center vertically
            glm::vec3 right = glm::normalize(glm::cross(r, camera.GetUp()));
            r = glm::rotate(r, dy * 0.05f, right);
            camera.SetPosition(camera.GetCenter() + r);
        }

        s_lastMouseX = x;
        s_lastMouseY = y;
        break;
    }

    case WM_MOUSEWHEEL:
    {
        // ref : https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-mousewheel
        short wheel = HIWORD(wParam);

        glm::vec3 disp = camera.GetCenter() - camera.GetPosition();

        camera.SetPosition(camera.GetPosition() - disp * 0.025f * wheel / WHEEL_DELTA);
        break;
    }

    case WM_KEYUP:
        if (wParam == VK_R)
        {
            camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }
        break;


    case WM_LBUTTONDOWN:case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:case WM_MBUTTONUP:
    case WM_KILLFOCUS: case WM_SETFOCUS:
    case WM_KEYDOWN: break;
    }
}

void GraphicsDemo::Render(GLuint program, Object& object)
{
    glUseProgram(program);

    GLint mvLocation = glGetUniformLocation(program, "mvMatrix");
    GLint projLocation = glGetUniformLocation(program, "projMatrix");
    GLint eyeLocation = glGetUniformLocation(program, "eyeMatrix");

    glUniformMatrix4fv(mvLocation, 1, GL_FALSE, (float*)&object.GetTransformMatrix()[0][0]);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, (float*)&camera.ProjectionMatrix()[0][0]);
    glUniformMatrix4fv(eyeLocation, 1, GL_FALSE, (float*)&camera.EyeMatrix()[0][0]);

    object.Render();
}
