#ifndef WINDOWS_INPUT_H_INCLUDE_GUARD
#define WINDOWS_INPUT_H_INCLUDE_GUARD

//------------------------------------------------------------


#include <queue>
#include <functional>
#include <algorithm>
#include <Windows.h>
#include "VkCode.h"


//------------------------------------------------------------


// Stocks up translated windows input messages.
class WindowsInput
{
      //------------------------------------------------------------
      // Defs
      //------------------------------------------------------------
public:
      struct VkEvent // virtual key event
      {
            enum State { S_UP, S_DOWN }   state;
            VkCode                        code;
            VkEvent(State state, VkCode code) : state(state), code(code) {}
      };

private:
      typedef std::vector<VkEvent> VkEventVec;



      //------------------------------------------------------------
      // Funcs
      //------------------------------------------------------------
public:
      void TranslateInput(UINT msg, DWORD wParam, DWORD lParam)
      {
            switch (msg)
            {
            case WM_KEYDOWN:
                  _veq.emplace_back(VkEvent::S_DOWN, wParam);
                  break;

            case WM_LBUTTONDOWN:
                  _veq.emplace_back(VkEvent::S_DOWN, VK_LBUTTON);
                  break;

            case WM_MBUTTONDOWN:
                  _veq.emplace_back(VkEvent::S_DOWN, VK_MBUTTON);
                  break;

            case WM_RBUTTONDOWN:
                  _veq.emplace_back(VkEvent::S_DOWN, VK_RBUTTON);
                  break;



            case WM_KEYUP:
                  _veq.emplace_back(VkEvent::S_UP, wParam);
                  break;

            case WM_LBUTTONUP:
                  _veq.emplace_back(VkEvent::S_UP, VK_LBUTTON);
                  break;

            case WM_MBUTTONUP:
                  _veq.emplace_back(VkEvent::S_UP, VK_MBUTTON);
                  break;

            case WM_RBUTTONUP:
                  _veq.emplace_back(VkEvent::S_UP, VK_RBUTTON);
                  break;

            case WM_MOUSEMOVE:
                  {
                        _dx = LOWORD(lParam) - _mouseX;
                        _dy = HIWORD(lParam) - _mouseY;
                        _mouseX = LOWORD(lParam);
                        _mouseY = HIWORD(lParam);
                        break;
                  }

            }
      }
      void ForAllEvent(std::function<void(VkEvent& ve)> fProc)
      {
            std::for_each(_veq.begin(), _veq.end(), fProc);
      }
      void Flush(void)
      {
            _veq.clear();
            _dx = 0;
            _dy = 0;
      }
      int MouseX(void)
      {
            return _mouseX;
      }
      int MouseY(void)
      {
            return _mouseY;
      }
      int MouseDx(void)
      {
            return _dx;
      }
      int MouseDy(void)
      {
            return _dy;
      }


      //------------------------------------------------------------
      // Vars
      //------------------------------------------------------------
private:
      //
      // Mouse Keyboard Inputs
      //
      VkEventVec        _veq; // virtual key eventes that will be stocking keyboard, mouse button pressing releasing events
      int               _mouseX; // X coordinate of mouse cursor
      int               _mouseY; // Y coordinate of mouse cursor
      int               _dx; // accumulative displacement in x
      int               _dy; // accumulative displacement in y
      

};

#endif

