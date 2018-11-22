#include "Application.h"

//D3D12 Test
#include "D3D12HelloTriangle.h"
#include "Platform.h"

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

    Platform::InitPlatformApplication(&sample);
    sample.OnInit();
    Platform::RunPlatformApplication();

    while (true);
  }

}
