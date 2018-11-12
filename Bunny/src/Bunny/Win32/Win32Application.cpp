#include <string>
#include "Win32Application.h"
#include "../DirectX12/TUT/D3D12HelloTriangle.h"


namespace Bunny {

  HWND Win32Application::m_hWnd = nullptr;

  Win32Application::Win32Application()
  {
  }


  Win32Application::~Win32Application()
  {
  }

  HWND Win32Application::GetHwnd()
  {
    return m_hWnd;
  }

  void Win32Application::CreateWindowHandle(D3D12HelloTriangle* app)
  {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
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

  void Win32Application::Run()
  {
    ShowWindow(m_hWnd, 10);
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
