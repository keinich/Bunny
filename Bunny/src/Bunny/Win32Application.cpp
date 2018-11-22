#include <string>
#include "Win32Application.h"
#include "D3D12HelloTriangle.h"
#include "Win32Window.h"


namespace Bunny {
  namespace Platform {
    namespace Win32 {

      Win32Window Win32Application::window_;
      Win32Window Win32Application::window2_;

      Win32Application::Win32Application()
      {
      }


      Win32Application::~Win32Application()
      {
      }

      void Win32Application::Init(void* app)
      {
        window_.Create(app);
        window2_.Create(app);
      }

      void Win32Application::Run()
      {
        ShowWindow(window_.GetWindowHandle(), 10);
        ShowWindow(window2_.GetWindowHandle(), 10);
        MSG msg = {};
        while (msg.message != WM_QUIT)
        {
          if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
          {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }
        }
      }

      LRESULT Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
      {
        D3D12HelloTriangle* app = reinterpret_cast<D3D12HelloTriangle*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        switch (message)
        {
        case WM_CREATE:
        {
          LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
          SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
        return 0;
        case WM_PAINT:
          if (app)
          {
            //app->OnUpdate();
            app->OnRender();
          }
          return 0;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
      }

    }
  }
}