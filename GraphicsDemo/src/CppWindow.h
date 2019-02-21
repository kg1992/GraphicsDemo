//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 

#ifndef CPP_WINDOW_H_INCLUDE_GUARD_H
#define CPP_WINDOW_H_INCLUDE_GUARD_H

//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unordered_map>
#include <memory>
#include <functional>

//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 

class CppWindow
{
      //------------------------------------------------------------
      // Defs
      //------------------------------------------------------------

public:
      // Callback function type that will be used in message
      typedef std::function<void(WORD code, HWND hControlWnd)> ControlFunc;
      
      // Window Instance Dependent WndProc
      typedef std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> WndProcType;

private:
      // Stores callbakc functions for control
      typedef std::unordered_map<HWND, ControlFunc> ControlFuncMap;

      // CppWindow Instanfce Container.
      typedef std::unordered_map<HWND, std::unique_ptr<CppWindow>> WndMap;

public:
      // Config for window generation
      struct Config
      {
            Config( void ) :
                  x(CW_USEDEFAULT),
                  y(CW_USEDEFAULT),
                  width(CW_USEDEFAULT),
                  height(CW_USEDEFAULT),
                  dwExStyle(NULL),
                  dwStyle(CS_HREDRAW | CS_VREDRAW),
                  fWndProc(nullptr)
            {
                  ua_tcscpy_s(wndName,0x100,L"Default Cpp Window");
            }
            static Config LoadFrom( LPCTSTR path );
            static void   SaveTo( LPCTSTR path );
            
            TCHAR       wndName[0x100];
            int         x;
            int         y;
            int         width;
            int         height;
            DWORD       dwExStyle;
            DWORD       dwStyle;
            WndProcType fWndProc;
      };
      
private:
      // Context of the application.
      struct Context
      {
            Context( void ) : 
                  hInst(NULL),
                  nCmdShow(SW_SHOW)
            {
            }
            HINSTANCE   hInst;
            int         nCmdShow;
      };

      //------------------------------------------------------------
      // Funcs
      //------------------------------------------------------------
public:
      //
      // CppWindow Control
      //
      // Call this function at the start of the application with given arguments to main
      static void       InitContext ( HINSTANCE hInst, int nCmdShow );

      // Create and returns handle to window.
      static HWND       Create      ( const Config& config = Config() );

      // Gets CppWindow interface to the window with handle specified.
      static CppWindow& GetWnd      ( HWND hWnd );
      static bool       HasWnd      ( HWND hWnd );

      // Returnes handle to window
      HWND              Handle      ( void );

      // Processes message. Blocks the process.
      BOOL              GetAndProcessMsg( void );

      // Processes message. Do not block the process.
      void              PeekAndProcessMsg( void );

      // Checks if is CppWindow on. CppWindow is off if window is destroyed.
      bool              IsOn        ( void );

      // Closes the window.
      void              Close       ( void );

      //
      // Window Utilites
      //
      RECT              ClientRect  ( void );
      //
      // Control Utilites
      //

      // Adds Push Button
      HWND              AddPushButton( int x, int y, int width, int height, ControlFunc fControlFunc );

      // Adds Rich Edit
      HWND              AddRichEdit(  int x, int y, int width, int height );

      // Add List Box
      HWND              AddListBox( int x, int y, int width, int height );


private:
      // CppWndProc. This will alwyas be the WndProc for CppWindow
      static LRESULT CALLBACK CppWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

      // No Construction, No Copy
                        CppWindow( HWND hWnd );
                        CppWindow( const CppWindow& rhs );

      //------------------------------------------------------------
      // Vars
      //------------------------------------------------------------
private:
      // CppWindow Instance Container. CppWindow will ONLY exist in this map
      static WndMap     _s_wndMap;
      // Context of the application. holds instance to application, window show flag
      static Context    _s_context;
      // Handle to the window tha this CppWindow instance represents
      HWND              _hWnd;
      // WndProc depend on the specific instance of CppWindow
      WndProcType       _proc;
      // If the Window is on. Window is off if window is destroyed sending WM_DESTROY
      bool              _isOn;
      // Remebers Control and its corresponding function
      ControlFuncMap    _controlFuncMap;

}; // class CppWindow

//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 

#endif //CPP_WINDOW_H_INCLUDE_GUARD_H

//-= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= 