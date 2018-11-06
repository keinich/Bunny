//Windows Includes
#include <windows.h>
#include <wrl.h>
#include <winerror.h>

//DirectX Includes
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi1_4.h>

//Bunny Includes
#include "d3dx12.h"
#include "src\Bunny\Win32\Win32Application.h"

//using namespace DirectX;
using namespace Microsoft::WRL;

namespace Bunny {
  namespace DirectX12 {


    class D3D12HelloTriangle {

    public:

      void static Run() {
        Win32Application::CreateWindowHandle();

        Win32Application::Run();
      }

    private:

      void LoadPipeline()
      {
        UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
        {
          ComPtr<ID3D12Debug> debugController;
          if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
          {
            debugController->EnableDebugLayer();

            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
          }
        }
#endif



      }

    };

  }
}