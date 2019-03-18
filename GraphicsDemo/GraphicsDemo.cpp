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
    const float CameraWalkSpeed = 10.0f;

    FontRenderer s_fontRenderer;
    GLuint s_cubeMap;
    bool s_bCommandInputState = false;
    std::string s_command;
    float s_lastTime;

    GLuint LoadCubeMap()
    {
        stbi_set_flip_vertically_on_load(false);
        const std::string SkyboxLocation = "./Resources/Skybox/mp_orbital/";
        const std::string SkyboxName = "orbital-element";
        const std::string Suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
        const std::string Extension = ".tga";
        
        GLuint t;
        glGenTextures(1, &t);
        GET_AND_HANDLE_GL_ERROR();

        glBindTexture(GL_TEXTURE_CUBE_MAP, t);
        GET_AND_HANDLE_GL_ERROR();

        for (int i = 0; i < _countof(Suffixes); ++i)
        {
            GLint w, h, n;
            const std::string texName = SkyboxLocation + SkyboxName + "_" + Suffixes[i] + Extension;
            stbi_uc* pData = stbi_load(texName.c_str(), &w, &h, &n, 0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
            GET_AND_HANDLE_GL_ERROR();
            stbi_image_free(pData);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        GET_AND_HANDLE_GL_ERROR();

        return t;
    }

    void DisplayFps()
    {
        float time = System::Instance()->CurrentTime();
        std::stringstream ss;
        ss << "FPS : " << std::round(1.0f / (time - s_lastTime));
        std::string s = ss.str();
        s_fontRenderer.RenderText(std::u32string(s.begin(), s.end()), 0, 100);
        s_lastTime = time;
    }

    void DisplayCommand()
    {
        if (s_bCommandInputState)
        {
            auto displayText = ">" + s_command;
            s_fontRenderer.RenderText(std::u32string(displayText.begin(), displayText.end()), 0, 300 - 32);
        }
    }

    std::vector<std::string> TokenizeCommand(const std::string& str)
    {
        std::vector<std::string> items;
        std::string token;
        bool InQuotation = false;
        token.reserve(0x100);
        for (int i = 0; i < str.size(); ++i)
        {
            char c = str[i];

            if (!InQuotation)
            {
                if (c == '"')
                {
                    InQuotation = true;
                    continue;
                }

                if (std::isspace(c, std::locale("C")))
                {
                    if (!token.empty())
                    {
                        items.push_back(token);
                        token.resize(0);
                        continue;
                    }
                }
            }
            else
            {
                if (c == '"')
                {
                    items.push_back(token);
                    token.resize(0);
                    InQuotation = false;
                    continue;
                }
            }

            token += c;
        }

        if (!token.empty())
        {
            items.push_back(token);
        }

        return std::move(items);
    }

    bool CaseInsensitiveCompare(const std::string& lhs, const std::string& rhs)
    {
        static std::locale s_locale("C");
        for (int i = 0; i < lhs.size(); ++i)
        {
            if (std::tolower(lhs[0], s_locale) != std::tolower(rhs[0], s_locale))
            {
                return false;
            }
        }
        return true;
    }
}

GraphicsDemo::GraphicsDemo()
    : m_lastMouseX(0)
    , m_lastMouseY(0)
    , m_mousePosRecordStarted(false)
    , m_pScene(new KnightPunchingScene())
{
}

void GraphicsDemo::OnStart()
{
    // Load, compile, link shader programs.
    ShaderPrograms::Init();
    
    m_pScene->Init();

    s_cubeMap = LoadCubeMap();

    s_fontRenderer.SetFont("./fonts/times.ttf");
    s_fontRenderer.SetGlyphSize(48);
}

void GraphicsDemo::Update(float dt)
{
    m_pScene->Update();

    float time = System::Instance()->CurrentTime() * 0.5f;
    float radius = 100.f;
    float x = cosf(time) * radius;
    float y = 150.f;
    float z = sinf(time) * radius;
    m_pScene->GetPointLight(0).position = glm::vec4(x, y, z, 1.0f);
}

void GraphicsDemo::Render()
{
    glViewport(0, 0, m_clientWidth, m_clientHeight);
    GET_AND_HANDLE_GL_ERROR();

    glClearColor(0, 0.8f, 0.7f, 1.0f);
    GET_AND_HANDLE_GL_ERROR();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    m_skyboxRenderer.Render(GetCamera(), s_cubeMap);

    m_sceneRenderer.Render(m_pScene);

    m_gizmoRenderer.Render(m_pScene);

    DisplayFps();

    DisplayCommand();

    glClear(GL_DEPTH_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    m_peekViewportRenderer.Render(m_pScene);
}

void GraphicsDemo::Free()
{
    m_pScene->Free();
    
    if (s_cubeMap)
    {
        glDeleteTextures(1, &s_cubeMap);
        s_cubeMap = 0;
    }
}



// Ref: https://docs.microsoft.com/en-us/windows/desktop/inputdev/using-keyboard-input
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

        s_fontRenderer.SetScreenSize(static_cast<float>(width), static_cast<float>(height));

        m_peekViewportRenderer.SetClientRect(width, height);

        if (m_pScene)
        {
            GetCamera().SetFrustum(glm::pi<float>() * 0.25f, width / 0.5f, height / 0.5f,
                0.1f, 10000.0f);
        }
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
            r = glm::rotate(r, -dx * Speed, GetCamera().GetReferenceUp());

            // revolve around center vertically
            glm::vec3 right = glm::normalize(glm::cross(r, GetCamera().GetReferenceUp()));
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

    case WM_CHAR:
        switch (wParam)
        {
        case 0x08:
            // Process a backspace.
            if (s_bCommandInputState && !s_command.empty())
                s_command.pop_back();
            break;

        case 0x0A:
            // Process a linefeed.
            break;

        case 0x1B:
            // Process an escape. 
            if (s_bCommandInputState)
            {
                s_bCommandInputState = false;
            }
            break;

        case 0x09:
            // Process a tab. 
            break;

        case 0x0D:
            // Process a carriage return. 
            if (s_bCommandInputState)
            {
                std::vector<std::string> tokens(TokenizeCommand(s_command));
                if (tokens.size() > 0 && CaseInsensitiveCompare(tokens[0], "load"))
                {
                    if (tokens.size() > 1 && CaseInsensitiveCompare(tokens[1], "scene"))
                    {
                        if (tokens.size() > 2)
                        {
                            LoadScene(tokens[2]);
                        }
                    }
                }

                if (tokens.size() > 0 && CaseInsensitiveCompare(tokens[0], "save"))
                {
                    if (tokens.size() > 1 && CaseInsensitiveCompare(tokens[1], "scene"))
                    {
                        if (tokens.size() > 2)
                        {
                            SaveScene(tokens[2]);
                        }
                    }
                }

                s_command.resize(0);
                s_bCommandInputState = false;
            }
            break;

        default:
            // Process displayable characters. 
            if (s_bCommandInputState)
            {
                const char keyCode = static_cast<char>(wParam);
                s_command.push_back(keyCode);
            }

            break;
        }
        
        break;

    case WM_KEYDOWN:
    {
        if (s_bCommandInputState)
        {
            return;
        }

        Camera& camera = GetCamera();
        glm::vec3 disp(0.0f);
        if (wParam == VK_W)
        {
            disp += camera.Forward();
        }
        if (wParam == VK_A)
        {
            disp += glm::cross(camera.GetReferenceUp(), camera.Forward());
        }
        if (wParam == VK_S)
        {
            disp -= camera.Forward();
        }
        if (wParam == VK_D)
        {
            disp -= glm::cross(camera.GetReferenceUp(), camera.Forward());
        }
        if (wParam == VK_SPACE)
        {
            disp += camera.GetReferenceUp();
        }
        if (wParam == VK_SHIFT)
        {
            disp -= camera.GetReferenceUp();
        }
        if (glm::dot(disp, disp) != 0.0f)
        {
            camera.MoveBy(glm::normalize(disp) * CameraWalkSpeed);
        }
    }
        break;

    case WM_KEYUP:
        if (s_bCommandInputState)
            return;

        if (wParam == VK_R)
        {
            GetCamera().LookAt(glm::vec3(50, 50, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }

        if (wParam == VK_OEM_3)
        {
            s_bCommandInputState = true;
        }
        
        break;


    case WM_LBUTTONDOWN:case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:case WM_MBUTTONUP:
    case WM_KILLFOCUS: case WM_SETFOCUS:
        break;
    }
}

bool GraphicsDemo::LoadScene(const std::string& sceneName)
{
    const char* const OutPath = "Resources/Scene/";
    const char* const Extension = ".dat";
    const std::string Filename = OutPath + sceneName + Extension;
    
    std::ifstream ifs(Filename, std::ios_base::in | std::ios_base::binary);
    if (!ifs)
        return false;
    
    std::shared_ptr<Scene> pScene(new Scene);
    pScene->Deserialize(ifs);
    ifs.close();

    m_pScene->Free();
    m_pScene = pScene;

    return true;
}

bool GraphicsDemo::SaveScene(const std::string& sceneName)
{
    const char* const OutPath = "Resources/Scene/";
    const char* const Extension = ".dat";
    const std::string Filename = OutPath + sceneName + Extension;
    std::ofstream ofs(Filename, std::ios::binary | std::ios::out);
    if (!ofs)
        return false;
    m_pScene->Serialize(ofs);
    return true;
}

Camera& GraphicsDemo::GetCamera()
{
    return m_pScene->GetCamera();
}