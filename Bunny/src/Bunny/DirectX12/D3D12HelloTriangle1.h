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
#include <DirectXMath.h>

//Bunny Includes
#include "d3dx12.h"
#include "..\Helpers.h"
#include "..\Win32\Win32Application.h"

//using namespace DirectX;
using namespace Microsoft::WRL;


namespace Bunny {
  namespace DirectX12 {


    class D3D12HelloTriangle1 {

    public:

      void Run();
        

      void OnRender()
      {
        PopulateCommandList();

        // Execute the command list.
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Present the frame.
        Helpers::ThrowIfFailed(m_swapChain->Present(1, 0));

        WaitForPreviousFrame();
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
            IID_PPV_ARGS(&m_device)
          ));
        }

        //command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        Helpers::ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

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
          m_commandQueue.Get(),
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
          Helpers::ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

          m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        }

        //frame resources
        {
          CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

          for (UINT n = 0; n < 2; n++) // 2 = FrameCount
          {
            Helpers::ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
          }
        }

        Helpers::ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

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
          Helpers::ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
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

          D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
          {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
          };

          D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
          psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
          psoDesc.pRootSignature = m_rootSignature.Get();
          psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
          psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
          psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
          psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
          psoDesc.DepthStencilState.DepthEnable = FALSE;
          psoDesc.DepthStencilState.StencilEnable = FALSE;
          psoDesc.SampleMask = UINT_MAX;
          psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
          psoDesc.NumRenderTargets = 1;
          psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
          psoDesc.SampleDesc.Count = 1;
          Helpers::ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        }

        // command list
        Helpers::ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

        Helpers::ThrowIfFailed(m_commandList->Close());

        // vertex buffer
        {
          Vertex triangleVertices[] =
          {
            { { 0.0f, 0.25f * (800 / 600), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * (800 / 600), 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * (800 / 600), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
          };

          const UINT vertexBufferSize = sizeof(triangleVertices);

          Helpers::ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));

          UINT8* pVertexDataBegin;
          CD3DX12_RANGE readRange(0, 0);
          Helpers::ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
          memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
          m_vertexBuffer->Unmap(0, nullptr);

          m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
          m_vertexBufferView.StrideInBytes = sizeof(Vertex);
          m_vertexBufferView.SizeInBytes = vertexBufferSize;

        }

        {
          Helpers::ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
          m_fenceValue = 1;

          m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
          if (m_fenceEvent == nullptr)
          {
            Helpers::ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
          }

          WaitForPreviousFrame();
        }

      }

      void WaitForPreviousFrame()
      {
        // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
        // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
        // sample illustrates how to use fences for efficient resource usage and to
        // maximize GPU utilization.

        // Signal and increment the fence value.
        const UINT64 fence = m_fenceValue;
        Helpers::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
        m_fenceValue++;

        // Wait until the previous frame is finished.
        if (m_fence->GetCompletedValue() < fence)
        {
          Helpers::ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
          WaitForSingleObject(m_fenceEvent, INFINITE);
        }

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
      }

      void PopulateCommandList()
      {
        Helpers::ThrowIfFailed(m_commandAllocator->Reset());

        Helpers::ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

        m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
        m_commandList->RSSetViewports(1, &m_viewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };

        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        m_commandList->DrawInstanced(3, 1, 0, 0);

        // Indicate that the back buffer will now be used to present.
        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        Helpers::ThrowIfFailed(m_commandList->Close());
      }

      ComPtr<ID3D12Device> m_device;
      ComPtr<ID3D12CommandQueue> m_commandQueue;
      ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
      UINT m_rtvDescriptorSize;
      ComPtr<IDXGISwapChain3> m_swapChain;
      UINT m_frameIndex;
      ComPtr<ID3D12Resource> m_renderTargets[2]; //FrameCount
      ComPtr<ID3D12CommandAllocator> m_commandAllocator;
      ComPtr<ID3D12RootSignature> m_rootSignature;
      ComPtr<ID3D12PipelineState> m_pipelineState;
      ComPtr<ID3D12GraphicsCommandList> m_commandList;
      ComPtr<ID3D12Resource> m_vertexBuffer;
      D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
      UINT64 m_fenceValue;
      HANDLE m_fenceEvent;
      ComPtr<ID3D12Fence> m_fence;
      CD3DX12_VIEWPORT m_viewport;
      CD3DX12_RECT m_scissorRect;

      struct Vertex
      {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
      };

    };
  }
}