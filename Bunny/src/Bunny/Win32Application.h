#pragma once

#include <windows.h>
//#include <wrl.h>

class D3D12HelloTriangle;

namespace Bunny {
  namespace Platform {
    namespace Win32 {

      class Win32Application {
        friend class Win32Window;
      public:
        Win32Application();
        ~Win32Application();

        void static Init(void* app);
        void static Run();

        static inline Win32Window* GetMainWindow() { return &window_; }

      protected:
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

      private:
        static Win32Window window_;
        static Win32Window window2_;
      };

    }
  }
}
