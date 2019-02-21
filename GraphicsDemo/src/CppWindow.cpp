//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 

#include "CppWindow.h"
#include <Richedit.h>

#include <cassert>
#include <cstring>


//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 

CppWindow::WndMap CppWindow::_s_wndMap;
CppWindow::Context CppWindow::_s_context;

//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 



void CppWindow::InitContext( HINSTANCE hInst, int nCmdShow )
{
      _s_context.hInst = hInst;
      _s_context.nCmdShow = nCmdShow;
}


//------------------------------------------------------------



HWND CppWindow::Handle( void )
{
      return _hWnd;
}


//------------------------------------------------------------




HWND CppWindow::Create( const Config& config )
{
      DWORD err = 0;

      // Register Window Class
      WNDCLASSEX wc;
      wc.cbSize         = sizeof(wc);
      wc.style          = config.dwStyle;
      wc.lpfnWndProc    = CppWndProc;
      wc.cbClsExtra     = NULL;
      wc.cbWndExtra     = NULL;
      wc.hInstance      = _s_context.hInst;
      wc.hIcon          = LoadIcon(NULL,IDI_APPLICATION);
      wc.hCursor        = LoadCursor(_s_context.hInst, IDC_ARROW);
      wc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
      wc.lpszMenuName   = NULL;
      wc.hIconSm        = NULL;
      wc.lpszClassName  = config.wndName;
      ATOM ret = RegisterClassEx(&wc);
      if( ret == 0 )
      {
            err = GetLastError();
            throw err;
      }

      
      // add CppWindow to the map
      std::unique_ptr<CppWindow> pCppWnd(new CppWindow(NULL));
      pCppWnd->_proc = config.fWndProc;
      

      // Create Window
      HWND hWnd = CreateWindowEx(
            config.dwExStyle,
            wc.lpszClassName,
            config.wndName,
            WS_OVERLAPPEDWINDOW,
            config.x,
            config.y,
            config.width,
            config.height,
            NULL,
            NULL,
            _s_context.hInst,
            NULL);
      if( !hWnd )
      {
            err = GetLastError();
            throw err;
      }

      pCppWnd->_hWnd = hWnd;
      _s_wndMap[hWnd].reset(pCppWnd.release());

      // return handle
      return hWnd;

}


//------------------------------------------------------------




CppWindow& CppWindow::GetWnd( HWND hWnd )
{
      assert(HasWnd(hWnd));
      CppWindow* pWnd = _s_wndMap[hWnd].get();
      return *pWnd;
}



//------------------------------------------------------------



bool CppWindow::HasWnd ( HWND hWnd )
{
      return _s_wndMap.find(hWnd) != _s_wndMap.end();
}



//------------------------------------------------------------




BOOL CppWindow::GetAndProcessMsg( void )
{
      MSG msg;
      BOOL ret;
      ret = GetMessage(&msg, _hWnd, 0, 0);
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      return ret;
}



//------------------------------------------------------------




void CppWindow::PeekAndProcessMsg( void )
{
      MSG msg;
      while( PeekMessage(&msg, _hWnd, 0,0, PM_REMOVE) )
      {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
      }
}


//------------------------------------------------------------





bool CppWindow::IsOn( void )
{
      return _isOn;
}



//------------------------------------------------------------




void CppWindow::Close( void )
{
      CloseWindow(_hWnd);
}


//------------------------------------------------------------


RECT CppWindow::ClientRect( void )
{
      RECT rect;
      GetClientRect(_hWnd,&rect);
      return rect;
}



//------------------------------------------------------------


HWND CppWindow::AddPushButton( int x, int y, int width, int height, ControlFunc fControlFunc )
{
      HWND hButton = CreateWindow(
            TEXT("Button"),
            TEXT("Button"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x,
            y,
            width,
            height,
            Handle(),
            NULL,
            _s_context.hInst,
            NULL);

      ShowWindow(hButton,SW_SHOW);
      assert(hButton != NULL );
      _controlFuncMap[hButton] = fControlFunc;

      return hButton;
}



//------------------------------------------------------------



HWND CppWindow::AddRichEdit(  int x, int y, int width, int height )
{
      LoadLibrary(TEXT("Msftedit.dll"));

      HWND hwndEdit= CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("a"),
            ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, 
            x, y, width, height, 
            _hWnd, NULL, _s_context.hInst, NULL);

      return hwndEdit;
}



//------------------------------------------------------------



HWND CppWindow::AddListBox( int x, int y, int width, int height )
{
      HWND hList = CreateWindow(
            L"listbox", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
            x, y, width, height,Handle(),0,_s_context.hInst,NULL);
      return hList;
}



//------------------------------------------------------------


#undef min

LRESULT CALLBACK CppWindow::CppWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
      // Process
      switch( uMsg )
      {
      case WM_DESTROY:
            CppWindow::GetWnd(hWnd)._isOn = false;
            break;

      case WM_COMMAND:
            {
                  CppWindow& cwnd = CppWindow::GetWnd(hWnd);
                  HWND hControlWnd = (HWND)lParam;
                  auto f = cwnd._controlFuncMap[hControlWnd];
                  if( f ) f(HIWORD(wParam),hControlWnd);
            }
            break;

            // Last window messages are not handled

      }
      // Get Corresponding CppWindow
      if( CppWindow::HasWnd(hWnd) )
      {
            CppWindow& cwnd = CppWindow::GetWnd(hWnd);

            // Window Instance Dependent WndProc
            if( cwnd._proc )
                  return cwnd._proc(hWnd,uMsg,wParam,lParam);
      }

      return DefWindowProc(hWnd,uMsg,wParam,lParam);

}



//------------------------------------------------------------




// No Construction, No Copy
CppWindow::CppWindow( HWND hWnd ) : 
      _hWnd(hWnd),
      _proc(nullptr),
      _isOn(true),
      _controlFuncMap()
{
}