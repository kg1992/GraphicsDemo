#include "System.h"
#include <iostream>
#include "Errors.h"
#include "SystemComponent.h"

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

    for (auto pComponent : m_components)
    {
        pComponent->OnStart();
    }

    while (m_quit == false)
    {
        float passedTime;
        do { passedTime = m_clock.GetSec() - m_lastFrameEndTime; } while (passedTime < (1.0f / 60));
        int fps = static_cast<int>(std::round(1 / passedTime));
        m_lastFrameEndTime = m_clock.GetSec();

        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for (auto pComponent : m_components)
        {
            pComponent->Update(passedTime);
        }

        Render();
   
    }

    for (auto pComponent : m_components)
    {
        pComponent->OnEnd();
    }
}

void System::Free()
{
    while (!m_components.empty())
    {
        m_components.back()->Free();
        m_components.resize(m_components.size() - 1);
    }
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

float System::CurrentTime()
{
    return m_clock.GetSec();
}

void System::AddComponent(SystemComponent * pComponent)
{
    m_components.emplace_back(pComponent);
}

LRESULT CALLBACK System::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    System* pSystem = System::Instance();
    for (auto pComponent : pSystem->m_components)
    {
        pComponent->OnWndProc(hwnd, uMsg, wParam, lParam);
    }

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
    for (auto pComponent : m_components)
    {
        pComponent->OnBeforeRender();
    }

    
    for (auto pComponent : m_components)
    {
        pComponent->Render();
    }


    for (auto pComponent : m_components)
    {
        pComponent->OnAfterRender();
    }

    // Double Buffering
    SwapBuffers(m_hdc);
}
