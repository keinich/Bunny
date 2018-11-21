#include "Application.h"

//D3D12 Test
#include "D3D12HelloTriangle.h"

namespace Bunny {

  Application::Application()
  {
  }


  Application::~Application()
  {
  }

  void Application::Run()
  {

    D3D12HelloTriangle sample(1280, 720, L"D3D12 Hello Triangle");

    Win32Application::CreateWindowHandle(&sample);
    sample.OnInit();
    Win32Application::Run();

    while (true);
  }

}
