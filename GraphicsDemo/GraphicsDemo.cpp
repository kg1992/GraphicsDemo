/*
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
    enum VKCode
    {
        VK_0 = 0x30,
        VK_1 = 0x31,
        VK_2 = 0x32,
        VK_3 = 0x33,
        VK_4 = 0x34,
        VK_5 = 0x35,
        VK_6 = 0x36,
        VK_7 = 0x37,
        VK_8 = 0x38,
        VK_9 = 0x39,
        VK_A = 0x41,
        VK_B = 0x42,
        VK_C = 0x43,
        VK_D = 0x44,
        VK_E = 0x45,
        VK_F = 0x46,
        VK_G = 0x47,
        VK_H = 0x48,
        VK_I = 0x49,
        VK_J = 0x4A,
        VK_K = 0x4B,
        VK_L = 0x4C,
        VK_M = 0x4D,
        VK_N = 0x4E,
        VK_O = 0x4F,
        VK_P = 0x50,
        VK_Q = 0x51,
        VK_R = 0x52,
        VK_S = 0x53,
        VK_T = 0x54,
        VK_U = 0x55,
        VK_V = 0x56,
        VK_W = 0x57,
        VK_X = 0x58,
        VK_Y = 0x59,
        VK_Z = 0x5A
    };

    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
    const float CameraWalkSpeed = 10.0f;

    FontRenderer s_fontRenderer;
    GLuint s_cubeMap;
    bool s_bCommandInputState = false;
    std::string s_command;
    float s_lastTime;

    GLuint renderTexture;
    GLuint frameBuffer;
    GLuint depthBuffer;

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

    m_sceneRenderer.SetRendererProgram(ShaderPrograms::s_phong);
    
    m_pScene->Init();

    s_cubeMap = LoadCubeMap();

    s_fontRenderer.SetFont("./fonts/times.ttf");
    s_fontRenderer.SetGlyphSize(48);

    glGenFramebuffers(1, &frameBuffer);
    GET_AND_HANDLE_GL_ERROR();

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    GET_AND_HANDLE_GL_ERROR();

    glGenTextures(1, &renderTexture);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    GET_AND_HANDLE_GL_ERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_clientWidth, m_clientHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GET_AND_HANDLE_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GET_AND_HANDLE_GL_ERROR();

    // Bind the texture to the FBO 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
    GET_AND_HANDLE_GL_ERROR();

    // Create the depth buffer 
    glGenRenderbuffers(1, &depthBuffer);
    GET_AND_HANDLE_GL_ERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    GET_AND_HANDLE_GL_ERROR();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_clientWidth, m_clientHeight);
    GET_AND_HANDLE_GL_ERROR();

    // Bind the depth buffer to the FBO 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    GET_AND_HANDLE_GL_ERROR();

    // Set the target for the fragment shader outputs 
    GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBufs);
    GET_AND_HANDLE_GL_ERROR();
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
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // Sets viewport to entire window
    glViewport(0, 0, m_clientWidth, m_clientHeight);
    GET_AND_HANDLE_GL_ERROR();

    // Sets clear color to cyan
    glClearColor(0, 0.8f, 0.7f, 1.0f);
    GET_AND_HANDLE_GL_ERROR();

    // Clear screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    // Render skybox
    m_skyboxRenderer.Render(GetCamera(), s_cubeMap);

    // Render scene
    m_sceneRenderer.Render(m_pScene);

    // Render gizmo
    m_gizmoRenderer.Render(m_pScene);

    // Render FPS
    DisplayFps();

    // Render command input
    DisplayCommand();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GET_AND_HANDLE_GL_ERROR();
    
    RenderScreen();

    // Render peek viewports
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

    glDeleteTextures(1, &renderTexture);
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
}

// Ref: https://docs.microsoft.com/en-us/windows/desktop/inputdev/using-keyboard-input
void GraphicsDemo::OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
    {
        // Window is resized. This message is also created when window is first created and shown.
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Saved client width and height
        m_clientWidth = width;
        m_clientHeight = height;

        // Font renderer needs to know screen metrics
        s_fontRenderer.SetScreenSize(static_cast<float>(width), static_cast<float>(height));

        // Peek viewport renderer also needs to know screen metrics
        m_peekViewportRenderer.SetClientRect(width, height);

        // Camera needs to know screen metrics to generate projection matrix
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
        if (wParam == VK_Q)
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

void GraphicsDemo::RenderScreen()
{
    ShaderProgram program = ShaderPrograms::s_screen;
    program.Use();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GET_AND_HANDLE_GL_ERROR();
    glActiveTexture(GL_TEXTURE0);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    GET_AND_HANDLE_GL_ERROR();
    program.SendUniform("uScreen", 0);
    program.SendUniform("uResolution"
        , static_cast<float>(m_clientWidth)
        , static_cast<float>(m_clientHeight));
    program.SendUniform("uTime", System::Instance()->CurrentTime());

    // Sets viewport to entire window
    glViewport(0, 0, m_clientWidth, m_clientHeight);
    GET_AND_HANDLE_GL_ERROR();

    // Sets clear color to cyan
    glClearColor(0, 0.8f, 0.7f, 1.0f);
    GET_AND_HANDLE_GL_ERROR();

    // Clear screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    GET_AND_HANDLE_GL_ERROR();

    // clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();
}
