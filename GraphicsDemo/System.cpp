#include "System.h"
#include "Errors.h"
#include "GL/glad.h"
#include <TCHAR.h>
#include <fstream>
#include <iostream>

namespace
{
    GLuint s_program;
    GLuint s_vertexArrayObject;

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
            OutputDebugStringA(infoLog.c_str());
            OutputDebugStringA("\n");
        }
        return shader;
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

    OutputDebugStringA(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

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
    glDeleteVertexArrays(1, &s_vertexArrayObject);
    glDeleteProgram(s_program);

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

        std::cout << "GLAD initialized" << std::endl;

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
            OutputDebugStringA(GetProgramInfoLog(program).c_str());
            OutputDebugStringA("\n");
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        //glDeleteShader(tessControlShader);
        //glDeleteShader(tessEvalShader);
        //glDeleteShader(geometryShader);

        s_program = program;

        glGenVertexArrays(1, &s_vertexArrayObject);
        glBindVertexArray(s_vertexArrayObject);
        
        System::Instance()->m_hdc = hdc;
        System::Instance()->m_hglrc = hglrc;

        GLuint buffer;

        glGenBuffers(1, &buffer);
        
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glBufferStorage(GL_ARRAY_BUFFER, 1024 * 1024, NULL, GL_MAP_WRITE_BIT);

        const float data[] =
        {
             0.25, -0.25, 0.5, 1.0,
            -0.25, -0.25, 0.5, 1.0,
             0.25, 0.25, 0.5, 1.0
        };

        // Put the data into the buffer at offset zero
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
        
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
        // HANDLE Resize case here
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
    GLfloat currentTime = m_clock.GetSec();
    GLfloat color[] = { 0, 0.8f, 0.7f, 1.0f };
    GLfloat color2[] = { std::sin(currentTime) * 0.5f + 0.5f, std::cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };

    glPointSize(5.0f);
    glClearBufferfv(GL_COLOR, 0, color);

    glUseProgram(s_program);

    GLfloat attrib[] = {
        std::sin(currentTime) * 0.5f,
        std::cos(currentTime) * 0.6f,
        0.0f, 0.0f };

    glVertexAttrib4fv(0, attrib);
    glVertexAttrib4fv(1, color2);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawArrays(GL_PATCHES, 0, 3);

    // Double Buffering
    SwapBuffers(m_hdc);

}
