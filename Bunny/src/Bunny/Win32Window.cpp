#include "Win32Window.h"
#include "Win32Application.h"

namespace Bunny {
  namespace Platform {
    namespace Win32 {


      Win32Window::Win32Window() :
        m_hWnd(nullptr)
      {
      }

      Win32Window::~Win32Window()
      {
      }

      void Win32Window::Create(void* app)
      {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASSEX windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = Win32Application::WindowProc;
        windowClass.hInstance = hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"DXSampleClass";
        RegisterClassEx(&windowClass);

        RECT windowRect = { 0,0,800,600 };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        m_hWnd = CreateWindow(
          windowClass.lpszClassName,
          L"Test",
          WS_OVERLAPPEDWINDOW,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          windowRect.right - windowRect.left,
          windowRect.bottom - windowRect.top,
          nullptr,
          nullptr,
          hInstance,
          app //TODO what is this param?
        );
      }

    }
  }
}