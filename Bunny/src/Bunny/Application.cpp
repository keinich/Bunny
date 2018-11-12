#include "Application.h"

//D3D12 Test
#include "DirectX12/D3D12HelloTriangle1.h"

namespace Bunny {

  Application::Application()
  {
  }


  Application::~Application()
  {
  }

  void Application::Run()
  {
    //D3D12HelloTriangle::
    DirectX12::D3D12HelloTriangle1 test;
    test.Run();
    while (true);
  }

}
