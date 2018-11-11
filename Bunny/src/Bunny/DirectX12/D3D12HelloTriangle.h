#pragma once

//Windows Includes
#include <windows.h>
#include <wrl.h>
#include <winerror.h>

//DirectX Includes
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

//Bunny Includes
#include "d3dx12.h"
#include "src\Bunny\Helpers.h"
#include "src\Bunny\Win32\Win32Application.h"

//using namespace DirectX;
using namespace Microsoft::WRL;

namespace Bunny {
  namespace DirectX12 {


    class D3D12HelloTriangle {

    public:

      void Run() {
        Win32Application::CreateWindowHandle();
        LoadPipeline();
        LoadAssets();
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
        //factory
        ComPtr<IDXGIFactory4> factory;
        Helpers::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

        //device
        if (false) //use Warp Device
        {
        }
        else
        {
          ComPtr<IDXGIAdapter1> hardwareAdapter;
          GetHardwareAdapter(factory.Get(), &hardwareAdapter);

          Helpers::ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&s_device)
          ));
        }

        //command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        Helpers::ThrowIfFailed(s_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_commandQueue)));

        //swap chain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = 2; //FrameCount
        swapChainDesc.Width = 800;
        swapChainDesc.Height = 600;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        Helpers::ThrowIfFailed(factory->CreateSwapChainForHwnd(
          s_commandQueue.Get(),
          Win32Application::GetHwnd(),
          &swapChainDesc,
          nullptr,
          nullptr,
          &swapChain
        ));

        Helpers::ThrowIfFailed(swapChain.As(&m_swapChain));
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        // descriptor heaps
        {
          D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
          rtvHeapDesc.NumDescriptors = 2; //FrameCount
          rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
          rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
          Helpers::ThrowIfFailed(s_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

          m_rtvDescriptorSize = s_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        }

        //frame resources
        {
          CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

          for (UINT n = 0; n < 2; n++) // 2 = FrameCount
          {
            Helpers::ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            s_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
          }
        }

        Helpers::ThrowIfFailed(s_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

      }

      void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
      {
        ComPtr<IDXGIAdapter1> adapter;
        *ppAdapter = nullptr;

        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
        {
          DXGI_ADAPTER_DESC1 desc;
          adapter->GetDesc1(&desc);

          if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
          {
            continue;
          }

          if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
          {
            break;
          }
        }

        *ppAdapter = adapter.Detach();
      }

      void LoadAssets()
      {
        // root signature
        {
          CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
          rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

          ComPtr<ID3DBlob> signature;
          ComPtr<ID3DBlob> error;
          Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
          Helpers::ThrowIfFailed(s_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
        }

        //pipeline state
        {
          ComPtr<ID3DBlob> vertexShader;
          ComPtr<ID3DBlob> pixelShader;

#if defined (_DEBUG)
          UINT compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
          UNIT complierFlags = 0;
#endif

          Helpers::ThrowIfFailed(D3DCompileFromFile(L"D:\\Raftek\\Bunny\\Bunny\\src\\Bunny\\DirectX12\\Shaders\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compilerFlags, 0, &vertexShader, nullptr));
          Helpers::ThrowIfFailed(D3DCompileFromFile(L"D:\\Raftek\\Bunny\\Bunny\\src\\Bunny\\DirectX12\\Shaders\\shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compilerFlags, 0, &pixelShader, nullptr));
        }
      }

      ComPtr<ID3D12Device> s_device;
      ComPtr<ID3D12CommandQueue> s_commandQueue;
      ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
      UINT m_rtvDescriptorSize;
      ComPtr<IDXGISwapChain3> m_swapChain;
      UINT m_frameIndex;
      ComPtr<ID3D12Resource> m_renderTargets[2]; //FrameCount
      ComPtr<ID3D12CommandAllocator> m_commandAllocator;
      ComPtr<ID3D12RootSignature> m_rootSignature;

    };
  }
}