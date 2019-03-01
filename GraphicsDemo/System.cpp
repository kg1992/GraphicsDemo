#include "System.h"
#include "Errors.h"
#include <glad.h>
#include <TCHAR.h>
#include <fstream>
#include <iostream> 
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <fbxsdk.h>
#include "Mesh.h"

namespace
{
    GLuint s_program;
    GLuint s_buffer;
    GLuint s_vao;
    GLint s_mvLocation;
    GLint s_projLocation;
    GLint s_eyeLocation;
    GLuint s_texture;
    GLint s_samplerLocation;
    glm::mat4 s_projectionMatrix = glm::identity<glm::mat4>();
    glm::mat4 s_eyeMatrix = glm::identity<glm::mat4>();

    std::string GetShaderInfoLog(GLuint shader)
    {
        GLsizei bufSize;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

        std::vector<GLchar> buf;
        buf.resize(bufSize + 1);

        // Get InfoLog
        glGetShaderInfoLog(shader, bufSize, nullptr, (GLchar*)buf.data());

        // return buffer
        return std::string(buf.data());
    }

    std::string GetProgramInfoLog(GLuint program)
    {
        GLsizei bufSize;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

        std::vector<GLchar> buf;
        buf.resize(bufSize + 1);

        // Get InfoLog
        glGetProgramInfoLog(program, bufSize, nullptr, (GLchar*)buf.data());

        // return buffer
        return std::string(buf.data());
    }

    GLuint CompileShaderFromSourceFile(GLenum type, const std::string& fileName)
    {
        GLuint shader = glCreateShader(type);
        std::ifstream fin(fileName);
        std::vector<GLchar> source((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        GLchar* pSource = source.data();
        GLint sourceSize = static_cast<GLint>(source.size());
        glShaderSource(shader, 1, &pSource, &sourceSize);
        glCompileShader(shader);
        std::string infoLog = GetShaderInfoLog(shader);
        if (infoLog.size() != 0) {
            std::cout << infoLog << std::endl;
        }
        return shader;
    }

    void generate_texture(float* data, int width, int height)
    {
        int end = width * height;
        for (int i = 0; i < end; ++i)
        {
            int offset = i * 4;
            int region = i * 3 / end;
            if (region == 0)
            {
                data[offset] = 1.0f;
                data[offset + 1] = 0.0f;
                data[offset + 2] = 0.0f;
                data[offset + 3] = 1.0f;
            }
            else if (region == 1)
            {
                data[offset] = 0.0f;
                data[offset + 1] = 1.0f;
                data[offset + 2] = 0.0f;
                data[offset + 3] = 1.0f;
            }
            else if (region = 2)
            {
                data[offset] = 0.0f;
                data[offset + 1] = 0.0f;
                data[offset + 2] = 1.0f;
                data[offset + 3] = 1.0f;
            }
        }
    }
}

System::System() : m_hwnd(NULL), m_hdc(NULL), m_hglrc(NULL), m_quit(false), m_clock(), m_lastFrameEndTime(0)
{
}

bool System::Init()
{
    LPCTSTR lpszClassName = TEXT("GraphicsDemo WClass");
    LPCTSTR lpszWndName = TEXT("Graphics Demo Window");

    HINSTANCE inst = GetModuleHandle(NULL);
    if (inst == NULL)
    {
        HandleWinapiError(L"GetModuleHandle fail");
        return false;
    }

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    // https://docs.microsoft.com/en-us/windows/desktop/winmsg/window-class-styles
    // CS_OWNDC is necessary for OpenGL window
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszClassName = lpszClassName;
    wc.lpszMenuName = NULL;
    wc.hIconSm = NULL;

    ATOM acls = RegisterClassEx(&wc);
    if (acls == 0) {
        HandleWinapiError(L"RegisterClassEx fail.");
        return false;
    }

    HWND hwnd = CreateWindow(lpszClassName, lpszWndName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, inst, NULL);
    if (hwnd == NULL)
    {
        HandleWinapiError(L"CreateWindow fail.");
    }

    ShowWindow(hwnd, SW_SHOW);

    m_hwnd = hwnd;

    return true;
}

void System::Loop()
{
    m_lastFrameEndTime = m_clock.GetSec();

    while (m_quit == false)
    {
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Render();
        float passedTime;
        do { passedTime = m_clock.GetSec() - m_lastFrameEndTime; } while (passedTime < (1.0f / 60));
        int fps = static_cast<int>(std::round(1 / passedTime));
        m_lastFrameEndTime = m_clock.GetSec();
    }
}

void System::Free()
{
    glDeleteBuffers(1, &s_buffer);
    glDeleteVertexArrays(1, &s_vao);
    glDeleteProgram(s_program);
    glDeleteTextures(1, &s_texture);

    if (!wglMakeCurrent(NULL, NULL))
    {
        HandleError(L"wglMakeCurrent fail");
    }

    if (!wglDeleteContext(m_hglrc))
    {
        HandleError(L"wglDeleteContext fail");
    }

    m_hwnd = NULL;
    m_hdc = NULL;
    m_hglrc = NULL;
}

LRESULT CALLBACK System::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Check For Windows Messages
    switch (uMsg)
    {
        // Watch For Window Activate Message
    case WM_ACTIVATE:
    {
        // Check Minimization State
        WORD state = HIWORD(wParam);
        if (state == 0) {
            // Program Is Active
        }
        return 0;
    }

    case WM_SYSCOMMAND:
    {
        // Check System Calls
        switch (wParam)
        {
        case SC_SCREENSAVE: // Screensaver is Starting
        case SC_MONITORPOWER: // Monitor is Trying To Enter Powersave
            return 0; // Prevent
        }
        break;
    }

    case WM_CREATE:
    {
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
            PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
            32,                   // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   // Number of bits for the depthbuffer
            8,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC hdc = GetDC(hwnd);
        if (hdc == NULL)
        {
            HandleError(L"GetDC fail");
            return 0;
        }

        int pixelFormat = ChoosePixelFormat(hdc, &pfd);
        if (pixelFormat == 0)
        {
            DWORD error = GetLastError();
            std::wstring errorMessage = L"ChoosePixelFormat fail. Code : ";
            errorMessage += std::to_wstring(error);
            HandleError(errorMessage);
            return 0;
        }

        SetPixelFormat(hdc, pixelFormat, &pfd);

        // glrc = GL Rendering Context
        HGLRC hglrc = wglCreateContext(hdc);
        if (!wglMakeCurrent(hdc, hglrc))
        {
            HandleError(L"wglMakeCurrent fail");
            return 0;
        }

        gladLoadGL();

        std::string glVersion((char*)glGetString(GL_VERSION));
        BOOL result = SetWindowTextA(hwnd, (std::string("Graphics Demo ( OpenGL Version : ") + glVersion + " )").c_str());
        if (result == FALSE) {
            HandleError(L"SetWindowText Fail.");
        }
        std::cout << "OpenGL version : " << glVersion << std::endl;

        if (glDebugMessageCallback) {
            std::cout << "Register OpenGL debug callback " << std::endl;
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OpenglCallbackFunction, nullptr);
            GLuint unusedIds = 0;
            glDebugMessageControl(GL_DONT_CARE,
                GL_DONT_CARE,
                GL_DONT_CARE,
                0,
                &unusedIds,
                true);
        }
        else
            std::cout << "glDebugMessageCallback not available" << std::endl;

        System::Instance()->m_hdc = hdc;
        System::Instance()->m_hglrc = hglrc;

        GLuint vertexShader = CompileShaderFromSourceFile(GL_VERTEX_SHADER, "vs.glsl");
        GLuint fragmentShader = CompileShaderFromSourceFile(GL_FRAGMENT_SHADER, "fs.glsl");
        //GLuint tessControlShader = CompileShaderFromSourceFile(GL_TESS_CONTROL_SHADER, "tcs.glsl");
        //GLuint tessEvalShader = CompileShaderFromSourceFile(GL_TESS_EVALUATION_SHADER, "tes.glsl");
        //GLuint geometryShader = CompileShaderFromSourceFile(GL_GEOMETRY_SHADER, "gs.glsl");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        //glAttachShader(program, tessControlShader);
        //glAttachShader(program, tessEvalShader);
        //glAttachShader(program, geometryShader);
        glLinkProgram(program);

        std::string programInfoLog = GetProgramInfoLog(program);
        if (programInfoLog.size() != 0) {

            std::cout << " * Program Info Log : " << std::endl
                << GetProgramInfoLog(program) << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        //glDeleteShader(tessControlShader);
        //glDeleteShader(tessEvalShader);
        //glDeleteShader(geometryShader);

        s_mvLocation = glGetUniformLocation(program, "mvMatrix");
        GET_AND_HANDLE_GL_ERROR();
        s_projLocation = glGetUniformLocation(program, "projMatrix");
        GET_AND_HANDLE_GL_ERROR();
        s_eyeLocation = glGetUniformLocation(program, "eyeMatrix");
        GET_AND_HANDLE_GL_ERROR();

        s_program = program;

        glGenVertexArrays(1, &s_vao);
        GET_AND_HANDLE_GL_ERROR();

        glBindVertexArray(s_vao);
        GET_AND_HANDLE_GL_ERROR();

        static const GLfloat vertex_positions[] =
        {
            -0.25f,  0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f,  0.25f,

            -0.25f,  0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
             0.25f,  0.25f,  0.25f,

             0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f, -0.25f
        };

        glGenBuffers(1, &s_buffer);
        GET_AND_HANDLE_GL_ERROR();

        glBindBuffer(GL_ARRAY_BUFFER, s_buffer);
        GET_AND_HANDLE_GL_ERROR();

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
        GET_AND_HANDLE_GL_ERROR();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        GET_AND_HANDLE_GL_ERROR();

        glEnableVertexAttribArray(0);
        GET_AND_HANDLE_GL_ERROR();

        s_eyeMatrix = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(), glm::vec3(0, 1, 0));

        glGenTextures(1, &s_texture);
        GET_AND_HANDLE_GL_ERROR();

        glBindTexture(GL_TEXTURE_2D, s_texture);
        GET_AND_HANDLE_GL_ERROR();

        int width = 512;
        int height = 512;
        float* data = new float[width * height * 4];
        generate_texture(data, width, height);

        //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 256, 256);
        //GET_AND_HANDLE_GL_ERROR();

        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_FLOAT, data);
        //GET_AND_HANDLE_GL_ERROR();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
        GET_AND_HANDLE_GL_ERROR();
        glGenerateMipmap(GL_TEXTURE_2D);
        GET_AND_HANDLE_GL_ERROR();

        s_samplerLocation = glGetUniformLocation(s_program, "s");
        GET_AND_HANDLE_GL_ERROR();

        delete[] data;
    }
    break;

    case WM_CLOSE:
    {
        System::Instance()->SetQuitFlag();
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
    {
        short width = LOWORD(lParam);
        short height = HIWORD(lParam);
        s_projectionMatrix = glm::perspectiveFov(glm::pi<float>() * 0.25f, width / 0.5f, height / 0.5f,
            0.1f, 1000.0f);
        glViewport(0, 0, width, height);
        GET_AND_HANDLE_GL_ERROR();
    }

    case WM_QUIT:
        break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_KEYDOWN: case WM_KEYUP:
    case WM_KILLFOCUS: case WM_SETFOCUS:
        // TODO: handle input here
        break;

    }


    // Pass All Unhandled Messages To DefWindowProc
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void System::SetQuitFlag()
{
    m_quit = true;
}

void System::Render()
{
    GLfloat color[] = { 0, 0.8f, 0.7f, 1.0f };

    glClearBufferfv(GL_COLOR, 0, color);

    glUseProgram(s_program);

    const float M_PI = 3.14159f;
    const float currentTime = m_clock.GetSec();
    const float f = (float)currentTime * (float)M_PI * 0.1f;

    glm::mat4 mvMatrix = glm::identity<glm::mat4>();

    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    mvMatrix = glm::translate(mvMatrix, glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) *0.5f, sinf(1.3f*f)));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 45.0f / M_PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 81.0f / M_PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(s_mvLocation, 1, GL_FALSE, (float*)&mvMatrix[0][0]);
    glUniformMatrix4fv(s_projLocation, 1, GL_FALSE, (float*)&s_projectionMatrix[0][0]);
    glUniformMatrix4fv(s_eyeLocation, 1, GL_FALSE, (float*)&s_eyeMatrix[0][0]);
    // glUniform1i(s_samplerLocation, s_texture);

    glBindVertexArray(s_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Double Buffering
    SwapBuffers(m_hdc);
}
