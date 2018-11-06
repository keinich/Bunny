#pragma once

#include <windows.h>
//#include <wrl.h>

namespace Bunny {

  class Win32Application {
  public:
    Win32Application();
    ~Win32Application();

    void static CreateWindowHandle();
    void static Run();

  protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  private:
    static HWND m_hWnd;
  };

}
