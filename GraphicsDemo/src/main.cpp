//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 


#include "CppWindow.h"
#include "WindowsHelper.h"
#include <Richedit.h>
#include <algorithm>

//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 


namespace
{
    LRESULT MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }


} // namespace


//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 



INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nCmdShow)
{
    CppWindow::InitContext(hInst, nCmdShow);

    CppWindow::Config config;

    config.fWndProc = MyWndProc;

    HWND hWindow = CppWindow::Create(config);

    ShowWindow(hWindow, nCmdShow);

    CppWindow& cwin = CppWindow::GetWnd(hWindow);

    CppWindow::ControlFunc fOnPushButton = [](WORD code, HWND hControlWnd)
    {
        if (code == BN_CLICKED)
        {
            MessageBox(NULL, L"Clicked", L"Me", MB_OK);
        }
    };


    cwin.AddPushButton(100, 100, 200, 200, fOnPushButton);
    cwin.AddPushButton(100, 300, 200, 200, fOnPushButton);
    HWND hListBox = cwin.AddListBox(300, 100, 500, 500);


    while (cwin.IsOn())
    {
        cwin.PeekAndProcessMsg();
    }

    return 0;

} // main


//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 