#include "Display.h"
#include "Helpers.h"
#include "Win32Application.h"
#include "Win32Window.h"
#include "CommandListManager.h"

#include "DXSample.h" //TODO needed for ComPtr -> why???
//#include <wrl.h>
#include "DX12Core.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      Display::Display(UINT width, UINT height) :
        width_(width),
        height_(height),
        m_frameIndex(0),
        m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
        m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
        m_rtvDescriptorSize(0)
      {
      }

      Display::~Display()
      {
      }

      void Display::Init(Microsoft::WRL::ComPtr<IDXGIFactory4> &factory, Microsoft::WRL::ComPtr<ID3D12Device> &device, Platform::PLATFORM_WINDOW* window) {
        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = width_;
        swapChainDesc.Height = height_;
        swapChainDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //swapChainDesc.BufferCount = 3;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;


#ifdef BN_PLATFORM_WINDOWS
        ComPtr<IDXGISwapChain1> swapChain;
        Helpers::ThrowIfFailed(factory->CreateSwapChainForHwnd(
          Bunny::Graphics::DX12::Core::g_CommandManager.GetCommandQueue(),        // Swap chain needs the queue so that it can force a flush on it.
          window->GetWindowHandle(),
          &swapChainDesc,
          nullptr,
          nullptr,
          &swapChain
        ));

        // This sample does not support fullscreen transitions.
        Helpers::ThrowIfFailed(factory->MakeWindowAssociation(window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));
#endif

        Helpers::ThrowIfFailed(swapChain.As(&m_swapChain));
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        // Create descriptor heaps.
        {
          // Describe and create a render target view (RTV) descriptor heap.
          D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
          rtvHeapDesc.NumDescriptors = FrameCount;
          rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
          rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
          ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

          m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // Create frame resources.
        {
          CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

          // Create a RTV for each frame.
          for (UINT n = 0; n < FrameCount; n++)
          {
            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
          }
        }
      }
    }
  }
}