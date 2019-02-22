#include "System.h"
#include "Errors.h"
#include "GL.h"

System::System() : m_hwnd(NULL), m_hdc(NULL), m_hglrc(NULL), m_quit(false)
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
    MSG msg;
    while (m_quit == false)
    {
        ZeroMemory(&msg, sizeof(msg));
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Simply clear the window with red
        static const GLfloat red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, red);
    }
}

void System::Free()
{
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

        std::string glVersion((char*)glGetString(GL_VERSION));
        SetWindowTextA(hwnd, (std::string("Graphics Demo ( OpenGL Version : ") + glVersion + " )").c_str());

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