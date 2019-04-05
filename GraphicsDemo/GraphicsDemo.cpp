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
#include "VKCode.h"
#include "ScreenBuffer.h"
#include "Mesh.h"

namespace
{
    const int ActiveLightCount = 1;
    const int MaximumLightCount = 5;
    const float CameraWalkSpeed = 10.0f;

    FontRenderer s_fontRenderer;
    GLuint s_cubeMap;
    bool s_bCommandInputState = false;
    std::string s_command;
    float s_lastTime;

    ScreenBuffer s_screenBuffer;

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
    , m_pScene(
        //new ShadowTestScene()
        new KnightPunchingScene()
    )
    , m_activeCameraIndex(0)
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

    s_screenBuffer.Init(m_clientWidth, m_clientHeight);
}

void GraphicsDemo::Update(float dt)
{
    m_pScene->Update();
}

void GraphicsDemo::Render()
{
    ////////// Rendering shadow map //////////
    for (int i = 0; i < m_pScene->GetDirectionalLightCount(); ++i)
    {
        DirectionalLight& light = m_pScene->GetDirectionalLight(i);
        if (light.GetCastShadow())
        {
            std::shared_ptr<ShadowMap2D> pShadowMap = light.GetShadowMap();
            
            glBindFramebuffer(GL_FRAMEBUFFER, pShadowMap->GetFrameBuffer());
            GET_AND_HANDLE_GL_ERROR();
            glViewport(0, 0, pShadowMap->GetMapWidth(), pShadowMap->GetMapHeight());
            GET_AND_HANDLE_GL_ERROR();
            const float Distance = 600.0f;
            const float Boundary = 600.0f;
            const glm::mat4 V = glm::lookAtRH(-light.GetDirection(), glm::vec3(), glm::vec3(0, 1, 0));
            const glm::mat4 P = glm::orthoRH(-Boundary, Boundary, -Boundary, Boundary, -Distance, Distance);
            glClear(GL_DEPTH_BUFFER_BIT);
            GET_AND_HANDLE_GL_ERROR();

            ShaderProgram& program = ShaderPrograms::s_phong;
            program.Use();
            program.SendUniform("uShadowMapRender", GL_TRUE);

            for (int j = 0; j < m_pScene->GetSceneObjectCount(); ++j)
            {
                Object& object = *m_pScene->GetSceneObject(j);
                const glm::mat4 mv = V * object.GetTransformMatrix();
                program.SendUniform("uMvMatrix", 1, false, mv);
                glm::mat3x3 normalMatrix = glm::transpose(glm::inverse(mv));
                program.SendUniform("uNormalMatrix", 1, false, normalMatrix);
                program.SendUniform("uVpMatrix", 1, false, P);

                object.SendAnimationData(program);

                for (int k = 0; k < object.GetMeshCount(); ++k)
                {
                    std::shared_ptr<Mesh> mesh(object.GetMesh(k));
                    mesh->Apply(); 

                    for (int i = 0; i < mesh->GetSubMeshCount(); ++i)
                    {
                        Mesh::SubMesh subMesh = mesh->GetSubMesh(i);
                        glDrawArrays(GL_TRIANGLES, subMesh.begin, subMesh.vertCount);
                        GET_AND_HANDLE_GL_ERROR();
                    }
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            GET_AND_HANDLE_GL_ERROR();
        }
    }

    ////////// Render Scene //////////
    glBindFramebuffer(GL_FRAMEBUFFER, s_screenBuffer.GetFrameBuffer());
    GET_AND_HANDLE_GL_ERROR();

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
    m_sceneRenderer.Render(GetCamera(),m_pScene);

    // Render gizmo
    m_gizmoRenderer.Render(GetCamera(), m_pScene);

    // Render FPS
    DisplayFps();

    // Render command input
    DisplayCommand();

    // Reset Framebuffer to default ( Window client area )
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GET_AND_HANDLE_GL_ERROR();
    
    RenderScreen();

    // Render peek viewports
    m_peekViewportRenderer.Render(m_pScene);

    ////////// Peek ShadowMap //////////
    for (int i = 0; i < m_pScene->GetDirectionalLightCount(); ++i)
    {
        DirectionalLight& light = m_pScene->GetDirectionalLight(i);
        if (light.GetCastShadow())
        {
            std::shared_ptr<ShadowMap2D> pShadowMap = light.GetShadowMap();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            GET_AND_HANDLE_GL_ERROR();

            glViewport(0, 300, 200, 200);
            GET_AND_HANDLE_GL_ERROR();

            ShaderPrograms::s_texture.Use();
            glActiveTexture(GL_TEXTURE0);
            GET_AND_HANDLE_GL_ERROR();
            glBindTexture(GL_TEXTURE_2D, pShadowMap->GetDepthTexture());
            GET_AND_HANDLE_GL_ERROR();
            ShaderPrograms::s_texture.SendUniform("uTexture", 0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            GET_AND_HANDLE_GL_ERROR();
        }
    }
}

void GraphicsDemo::Free()
{
    m_pScene->Free();
    
    if (s_cubeMap)
    {
        glDeleteTextures(1, &s_cubeMap);
        s_cubeMap = 0;
    }

    s_screenBuffer.Free();
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
            const int CameraCount = m_pScene->GetCameraCount();
            for (int i = 0; i < CameraCount; ++i)
            {
                m_pScene->GetCamera(i).SetFrustum(glm::pi<float>() * 0.25f, 
                    static_cast<float>(width) / height,
                    0.1f, 10000.0f);
            }
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

        PerspectiveCamera& camera = GetCamera();
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
        if (wParam == VK_TAB)
        {
            m_activeCameraIndex = (m_activeCameraIndex + 1) % m_pScene->GetCameraCount();
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

PerspectiveCamera& GraphicsDemo::GetCamera()
{
    return m_pScene->GetCamera(m_activeCameraIndex);
}

void GraphicsDemo::RenderScreen()
{
    ShaderProgram& program = ShaderPrograms::s_screen;
    program.Use();
    glActiveTexture(GL_TEXTURE0);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, s_screenBuffer.GetTexture());
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
