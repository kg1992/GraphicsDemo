/*
    GraphicsDemo.cpp

    class GraphicsDemo implementation.

    Todo:
        utilaize ShaderProgram to send matrix uniforms.
*/
#include <iostream>
#include <sstream>
#include <glad.h>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Windowsx.h>
#include <WinUser.h>
#include "SystemComponent.h"
#include "GraphicsDemo.h"
#include "Errors.h"
#include "System.h"
#include "Object.h"
#include "stb_image.h"
#include "ShaderPrograms.h"

void LoadFbxSdkObject(const char* const filename, std::vector<std::shared_ptr<Object>>& objectStack);
std::shared_ptr<Mesh> GeneratePlane();

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

    const glm::vec4 s_worldLight(0, 50, 0, 1);
    const int ActiveLightCount = 1;
}

GraphicsDemo::GraphicsDemo()
    : m_clientWidth(0)
    , m_clientHeight(0)
    , m_lastMouseX(0)
    , m_lastMouseY(0)
    , m_mousePosRecordStarted(false)
{
}

void GraphicsDemo::OnStart()
{
    // Load, compile, link shader programs.
    ShaderPrograms::Init();
    
    PrepareLights();

    // Initial m_camera location.
    m_camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));

    // Load dragon form fbx file.
    LoadFbxSdkObject("./Resources/56-fbx/fbx/Dragon 2.5_fbx.fbx", m_objects);

    // Dragon is facing downward after loaded. I'm rotating it to make it face forward. (+Z)
    m_objects.back()->SetRotation(glm::angleAxis(glm::pi<float>()*-0.5f, glm::vec3(1, 0, 0)));

    AddGround();
}

void GraphicsDemo::Update(float dt)
{
    float time = System::Instance()->CurrentTime();
    float radius = 25.0f;
    float x = 0;// cosf(time) * radius;
    float y = (sinf(time) + 1) * radius;
    float z = 0;// sinf(time) * radius;
    m_pointLights[0].position = glm::vec4(x, y, z, 1.0f);
}

void GraphicsDemo::Render()
{
    glViewport(0, 0, m_clientWidth, m_clientHeight);
    GET_AND_HANDLE_GL_ERROR();
    glClearColor(0, 0.8f, 0.7f, 1.0f);
    GET_AND_HANDLE_GL_ERROR();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    DrawObjectCenter();

    DrawPointLights();

    DrawScene();

    glClear(GL_DEPTH_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    DrawPeekViewports();
    
}

void GraphicsDemo::Free()
{
    while (!m_objects.empty())
    {
        m_objects.back()->Free();
        m_objects.pop_back();
    }
}

void GraphicsDemo::OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        m_clientWidth = LOWORD(lParam);
        m_clientHeight = HIWORD(lParam);
        m_camera.SetFrustum(glm::pi<float>() * 0.25f, m_clientWidth / 0.5f, m_clientHeight / 0.5f,
            0.1f, 1000.0f);

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
            glm::vec3 disp = m_camera.GetCenter() - m_camera.GetPosition();
            if (dy > 0) {
                for (int i = 0; i < dy; ++i) {
                    m_camera.SetPosition(m_camera.GetPosition() - disp * 0.025f);
                }
            }
            else if (dy < 0)
            {
                for (int i = 0; i > dy; --i) {
                    m_camera.SetPosition(m_camera.GetPosition() + disp * 0.05f);
                }

            }
        }

        else if (wParam & MK_SHIFT && wParam & MK_MBUTTON)
        {
            glm::mat4x4 invEye = glm::inverse(m_camera.EyeMatrix());
            glm::vec4 ex = invEye[0];
            glm::vec4 ey = invEye[1];

            m_camera.MoveBy(ex.xyz * (float)dx);
            m_camera.MoveBy(ey.xyz * -(float)dy);
        }

        else if (wParam & MK_MBUTTON)
        {
            const float Speed = 0.02f;
            // relative displacement from center to eye
            glm::vec3 r = m_camera.GetPosition() - m_camera.GetCenter();

            // revolve around center horizontally
            r = glm::rotate(r, -dx * Speed, m_camera.GetUp());

            // revolve around center vertically
            glm::vec3 right = glm::normalize(glm::cross(r, m_camera.GetUp()));
            r = glm::rotate(r, dy * Speed, right);
            m_camera.SetPosition(m_camera.GetCenter() + r);
        }

        m_lastMouseX = x;
        m_lastMouseY = y;
        break;
    }

    case WM_MOUSEWHEEL:
    {
        // ref : https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-mousewheel
        short wheel = HIWORD(wParam);

        glm::vec3 disp = m_camera.GetCenter() - m_camera.GetPosition();

        m_camera.SetPosition(m_camera.GetPosition() + disp * 0.025f * wheel / WHEEL_DELTA);
        break;
    }

    case WM_KEYUP:
        if (wParam == VK_R)
        {
            m_camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }
        break;


    case WM_LBUTTONDOWN:case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:case WM_MBUTTONUP:
    case WM_KILLFOCUS: case WM_SETFOCUS:
    case WM_KEYDOWN: break;
    }
}

void GraphicsDemo::RenderObject(ShaderProgram& program, Object& object)
{
    program.Use();
    GET_AND_HANDLE_GL_ERROR();

    SendMaterial(program);

    SendMatrices(program, object);

    object.Render();
}

void GraphicsDemo::AddGround()
{
    // Ground object setup
    std::shared_ptr<Object> pGround(new Object);
    pGround->SetMesh(GeneratePlane());
    // Scale to cover large area
    pGround->SetScale(glm::vec3(100, 1, 100));
    m_objects.push_back(pGround);
}

void GraphicsDemo::DrawPointLights()
{
    for( int i = 0; i < ActiveLightCount; ++i )
    {
        PointLight& pointLight = m_pointLights[i];
        auto& program = ShaderPrograms::s_pointLight;

        program.Use();

        program.SendUniform("wCenterPos", glm::vec4(pointLight.position, 1));
        program.SendUniform("wScale", 5.0f, 5.0f);

        program.SendUniform("wvMatrix", 1, false, m_camera.EyeMatrix());
        program.SendUniform("projMatrix", 1, false, m_camera.ProjectionMatrix());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        GET_AND_HANDLE_GL_ERROR();
    }
}

void GraphicsDemo::DrawScene()
{
    ShaderProgram program = ShaderPrograms::s_phong;

    program.Use();

    SendLights(program, ActiveLightCount);

    for (auto& pObject : m_objects)
    {
        RenderObject(program, *pObject);
    }
}

void GraphicsDemo::DrawPeekViewports()
{
    int peekViewWidth = m_clientWidth / 8;
    int peekViewHeight = m_clientHeight / 8;
    ShaderProgram programs[] = { ShaderPrograms::s_position,
                                 ShaderPrograms::s_uv,
                                 ShaderPrograms::s_normal };
    for (int i = 0; i < 3; ++i)
    {
        glViewport(peekViewWidth * i, 0, peekViewWidth, peekViewHeight);
        GET_AND_HANDLE_GL_ERROR();

        for (auto& pObject : m_objects)
        {
            RenderObject(programs[i], *pObject);
        }
    }
}

void GraphicsDemo::DrawObjectCenter()
{
    ShaderProgram& program = ShaderPrograms::s_axes;

    program.Use();

    for (int i = 0; i < m_objects.size(); ++i)
    {
        std::shared_ptr<Object> pObject = m_objects[i];

        SendMatrices(program, *pObject);
        
        glm::vec3 position = pObject->GetPosition();
        
        glDrawArrays(GL_LINES, 0, 6);
    }
}

void GraphicsDemo::PrepareLights()
{
    m_pointLights.resize(5);

    for (int i = 0; i < 5; ++i)
    {
        std::stringstream name;
        name << "light[" << i << "].Position";
        float x = 2.0f * cosf((glm::two_pi<float>() / 5) * i) * 10.0f;
        float z = 2.0f * sinf((glm::two_pi<float>() / 5) * i) * 10.0f;
        m_pointLights[i].position = glm::vec4(x, 1.2f, z + 1.0f, 1.0f);
    }
    PointLight pl;

    m_pointLights[0].la = glm::vec3(0.0f, 0.2f, 0.2f);
    m_pointLights[1].la = glm::vec3(0.0f, 0.0f, 0.2f);
    m_pointLights[2].la = glm::vec3(0.2f, 0.0f, 0.0f);
    m_pointLights[3].la = glm::vec3(0.0f, 0.2f, 0.0f);
    m_pointLights[4].la = glm::vec3(0.2f, 0.2f, 0.2f);

    m_pointLights[0].ld = glm::vec3(0.0f, 0.8f, 0.8f);
    m_pointLights[1].ld = glm::vec3(0.0f, 0.0f, 0.8f);
    m_pointLights[2].ld = glm::vec3(0.8f, 0.0f, 0.0f);
    m_pointLights[3].ld = glm::vec3(0.0f, 0.8f, 0.0f);
    m_pointLights[4].ld = glm::vec3(0.8f, 0.8f, 0.8f);

    m_pointLights[0].ls = glm::vec3(0.0f, 0.8f, 0.8f);
    m_pointLights[1].ls = glm::vec3(0.0f, 0.0f, 0.8f);
    m_pointLights[2].ls = glm::vec3(0.8f, 0.0f, 0.0f);
    m_pointLights[3].ls = glm::vec3(0.0f, 0.8f, 0.0f);
    m_pointLights[4].ls = glm::vec3(0.8f, 0.8f, 0.8f);

    m_pointLights.push_back(pl);
}

void GraphicsDemo::SendPointLight(ShaderProgram& program, int index, PointLight& light)
{
    std::stringstream ss;

    const bool useFullSetting = false;

    if (useFullSetting)
    {
        ss << "light[" << index << "].position";
        program.SendUniform(ss.str().c_str(), m_camera.EyeMatrix() * glm::vec4(light.position, 1));
        std::stringstream().swap(ss);

        ss << "light[" << index << "].la";
        program.SendUniform(ss.str().c_str(), light.la);
        std::stringstream().swap(ss);

        ss << "light[" << index << "].ld";
        program.SendUniform(ss.str().c_str(), light.ld);
        std::stringstream().swap(ss);

        ss << "light[" << index << "].ls";
        program.SendUniform(ss.str().c_str(), light.ls);
        std::stringstream().swap(ss);
    }
    else
    {
        std::stringstream ss;

        ss << "light[" << index << "].position";
        program.SendUniform(ss.str().c_str(), m_camera.EyeMatrix() * glm::vec4(light.position, 1));
        std::stringstream().swap(ss);

        ss << "light[" << index << "].la";
        program.SendUniform(ss.str().c_str(), light.la);
        std::stringstream().swap(ss);

        ss << "light[" << index << "].l";
        program.SendUniform(ss.str().c_str(), light.ld);
        std::stringstream().swap(ss);
    }
}

void GraphicsDemo::SendLights(ShaderProgram& program, int count)
{
    for (int i = 0; i < count; ++i)
    {
        SendPointLight(program, i, m_pointLights[i]);
    }
}

void GraphicsDemo::SendMaterial(ShaderProgram& program)
{
    program.SendUniform("material.ka", 0.2f, 0.2f, 0.2f);
    program.SendUniform("material.kd", 0.4f, 0.4f, 0.4f);
    program.SendUniform("material.ks", 0.9f, 0.9f, 0.9f);
    program.SendUniform("material.shininess", 80.0f);
}

void GraphicsDemo::SendMatrices(ShaderProgram & program, Object& object)
{
    program.TrySendUniform("mwMatrix", 1, false, object.GetTransformMatrix());
    glm::mat4x4 mvMatrix = m_camera.EyeMatrix() * object.GetTransformMatrix();
    program.TrySendUniform("mvMatrix", 1, false, mvMatrix);
    glm::mat3x3 normalMatrix = glm::transpose(glm::inverse(mvMatrix));
    program.TrySendUniform("normalMatrix", 1, false, normalMatrix);
    program.TrySendUniform("projMatrix", 1, false, m_camera.ProjectionMatrix());
}
