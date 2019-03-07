/*
    main.cpp

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    References : 
     - Redirect cin and cout
     https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files

    Application entry point of GraphicsDemo project. 
     - (Debug mode) Creates Console and redirect stdout, stderr, stdin to the console
     - (Release mode) Redirect stdout, stderr to the file "log.txt".
                      log file will be generated next to exe file
     - Initialize 'System' class and its SystemComponents.
*/

#include "Common.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "System.h"
#include "GraphicsDemo.h"

void AllocateDebugConsole();
void BindCrtHandlesToStdHandles(bool, bool, bool);

namespace
{
    GD_TSTRING GetExePath()
    {
        TCHAR exeName[MAX_PATH];
        GetModuleFileName(NULL, exeName, MAX_PATH);
        return exeName;
    }

    GD_TSTRING GetLogFilePath()
    {
        GD_TSTRING path(GetExePath());
        path.resize(path.find_last_of('\\') + 1);
        path += TEXT("log.txt");
        return path;
    }

    void RedirectStandardOutput(std::ostream& os)
    {
        std::cout.rdbuf(os.rdbuf());
        std::cerr.rdbuf(os.rdbuf());
    }
}


// Entry point for Win32 Desktop Application.
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nCmdShow)
{
#ifdef GD_USE_CONSOLE
    // Use console buffer to print log
    AllocateDebugConsole();
    BindCrtHandlesToStdHandles(true, true, true);
#else
    // Use file buffer to print log
    std::ofstream logFile;
    logFile.open(GetLogFilePath());
    RedirectStandardOutput(logFile);
#endif

    // Log WinMain inputs. hPrev is always NULL
    std::cout
        << "hInst : " << args << std::endl
        << "ARGS : " << args << std::endl
        << "nCmdShow : " << nCmdShow << std::endl;

    System* system = System::Instance();

    system->AddComponent(new GraphicsDemo());

    system->Run();

    return 0;
}
