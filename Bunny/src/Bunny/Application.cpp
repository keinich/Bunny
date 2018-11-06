#include "Application.h"

//D3D12 Test
#include "DirectX12/D3D12HelloTriangle.h"

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
    D3D12HelloTriangle::Run();
    while (true);
  }

}
