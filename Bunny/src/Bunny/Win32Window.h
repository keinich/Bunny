#pragma once

#include <windows.h>

namespace Bunny {
  namespace Platform {
    namespace Win32 {

      class Win32Window
      {
        friend class Win32Application;
      public:
        Win32Window();
        ~Win32Window();

        void Create(void* app);
        HWND inline GetWindowHandle() { return m_hWnd; };

      protected:

      private:
        HWND m_hWnd;
      };

    }
  }
}
