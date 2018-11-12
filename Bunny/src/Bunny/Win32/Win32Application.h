#pragma once

#include <windows.h>
//#include <wrl.h>

namespace Bunny {

  namespace DirectX12 { class D3D12HelloTriangle; }

  class Win32Application {
  public:
    Win32Application();
    ~Win32Application();

    static HWND GetHwnd();

    void static CreateWindowHandle(class Bunny::DirectX12::D3D12HelloTriangle* app);
    void static Run();

  protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  private:
    static HWND m_hWnd;
  };

}
