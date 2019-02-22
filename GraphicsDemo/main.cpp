#include "System.h"

#pragma comment( lib, "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/SDK/ScopeCppSDK/SDK/lib/OpenGL32.lib")

// Entry point for Win32 Desktop Application.
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nCmdShow)
{
    System* system = System::Instance();

    system->Init();

    system->Loop();

    system->Free();

    return 0;

}
