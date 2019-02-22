#include "System.h"

#pragma comment( lib, "lib/x64/OpenGL32.lib")
#pragma comment( lib, "lib/x64/glew32.lib")

// Entry point for Win32 Desktop Application.
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nCmdShow)
{
    System* system = System::Instance();

    system->Init();

    system->Loop();

    system->Free();

    return 0;
}
