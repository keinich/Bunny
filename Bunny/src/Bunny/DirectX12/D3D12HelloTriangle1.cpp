#include "D3D12HelloTriangle1.h"
#include "TUT\D3D12HelloTriangle.h"

void Bunny::DirectX12::D3D12HelloTriangle1::Run()
{
  D3D12HelloTriangle sample(1280, 720, L"D3D12 Hello Triangle");

  Win32Application::CreateWindowHandle(&sample);
  sample.OnInit();
  //LoadPipeline();
  //LoadAssets();
  Win32Application::Run();
}