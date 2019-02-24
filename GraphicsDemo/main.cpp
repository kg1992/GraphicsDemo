#include "System.h"

#pragma comment( lib, "lib/x64/OpenGL32.lib")

void AllocateDebugConsole();
void BindCrtHandlesToStdHandles(bool, bool, bool);

// Entry point for Win32 Desktop Application.
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nCmdShow)
{

#ifdef _DEBUG
    AllocateDebugConsole();
    BindCrtHandlesToStdHandles(true, true, true);
#endif

    System* system = System::Instance();

    system->Init();

    system->Loop();

    system->Free();

    return 0;
}
