//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "stdafx.h"
#include "D3D12HelloTriangle.h"
#include "DX12Core.h"
#include "CommandListManager.h"
#include "GraphicsCore.h"
#include "Win32Window.h"
#include "Platform.h"

D3D12HelloTriangle::D3D12HelloTriangle(UINT width, UINT height, std::wstring name) :
  DXSample(width, height, name),
  theDisplay_(width, height)
{
}

void D3D12HelloTriangle::OnInit()
{
  LoadPipeline();
  LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D12HelloTriangle::LoadPipeline()
{
  UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
  // Enable the debug layer (requires the Graphics Tools "optional feature").
  // NOTE: Enabling the debug layer after device creation will invalidate the active device.
  {
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
      debugController->EnableDebugLayer();

      // Enable additional debug layers.
      dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
  }
#endif

  // factory
  ComPtr<IDXGIFactory4> factory;
  ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

  // device
  if (m_useWarpDevice)
  {
    ComPtr<IDXGIAdapter> warpAdapter;
    ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

    ThrowIfFailed(D3D12CreateDevice(
      warpAdapter.Get(),
      D3D_FEATURE_LEVEL_11_0,
      IID_PPV_ARGS(&m_device)
    ));
  }
  else
  {
    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(
      hardwareAdapter.Get(),
      D3D_FEATURE_LEVEL_11_0,
      IID_PPV_ARGS(&m_device)
    ));
  }

  // info queue
#if _DEBUG
  CreateInfoQueue();
#endif

  CheckTypedLoadUavSupport(); 

  // command queue
  Bunny::Graphics::DX12::Core::g_CommandManager.Create(m_device.Get());

  // swap chain and render target views an RTV descriptor heap is in the display
  theDisplay_.Init(factory, m_device, Bunny::Platform::GetMainPlatformWindow());

  ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

void D3D12HelloTriangle::CheckTypedLoadUavSupport()
{
  D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
  if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
  {
    if (FeatureData.TypedUAVLoadAdditionalFormats)
    {
      D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
      {
        DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
      };

      if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
        (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
      {
        m_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
      }

      Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

      if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
        (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
      {
        m_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
      }
    }
  }
}

void D3D12HelloTriangle::CreateInfoQueue()
{
  ID3D12InfoQueue* pInfoQueue = nullptr;
  if (SUCCEEDED(m_device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
  {
    // Suppress whole categories of messages
    //D3D12_MESSAGE_CATEGORY Categories[] = {};

    // Suppress messages based on their severity level
    D3D12_MESSAGE_SEVERITY Severities[] =
    {
      D3D12_MESSAGE_SEVERITY_INFO
    };

    // Suppress individual messages by their ID
    D3D12_MESSAGE_ID DenyIds[] =
    {
      // This occurs when there are uninitialized descriptors in a descriptor table, even when a
      // shader does not access the missing descriptors.  I find this is common when switching
      // shader permutations and not wanting to change much code to reorder resources.
      D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

      // Triggered when a shader does not export all color components of a render target, such as
      // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
      D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

      // This occurs when a descriptor table is unbound even when a shader does not access the missing
      // descriptors.  This is common with a root signature shared between disparate shaders that
      // don't all need the same types of resources.
      D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

      // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
      (D3D12_MESSAGE_ID)1008,
    };

    D3D12_INFO_QUEUE_FILTER NewFilter = {};
    //NewFilter.DenyList.NumCategories = _countof(Categories);
    //NewFilter.DenyList.pCategoryList = Categories;
    NewFilter.DenyList.NumSeverities = _countof(Severities);
    NewFilter.DenyList.pSeverityList = Severities;
    NewFilter.DenyList.NumIDs = _countof(DenyIds);
    NewFilter.DenyList.pIDList = DenyIds;

    pInfoQueue->PushStorageFilter(&NewFilter);
    pInfoQueue->Release();
  }
}

// Load the sample assets.
void D3D12HelloTriangle::LoadAssets()
{
  // Create an empty root signature.
  {
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
  }

  // Create the pipeline state, which includes compiling and loading shaders.
  {
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed(D3DCompileFromFile(L"D:\\Raftek\\Bunny\\Bunny\\src\\Bunny\\DirectX12\\TUT\\shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(L"D:\\Raftek\\Bunny\\Bunny\\src\\Bunny\\DirectX12\\TUT\\shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
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
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R10G10B10A2_UNORM;
    psoDesc.SampleDesc.Count = 1;
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
  }

  // Create the command list.
  ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

  // Command lists are created in the recording state, but there is nothing
  // to record yet. The main loop expects it to be closed, so close it now.
  ThrowIfFailed(m_commandList->Close());

  // Create the vertex buffer.
  {
    // Define the geometry for a triangle.
    Vertex triangleVertices[] =
    {
        { { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    const UINT vertexBufferSize = sizeof(triangleVertices);

    // Note: using upload heaps to transfer static data like vert buffers is not 
    // recommended. Every time the GPU needs it, the upload heap will be marshalled 
    // over. Please read up on Default Heap usage. An upload heap is used here for 
    // code simplicity and because there are very few verts to actually transfer.
    ThrowIfFailed(m_device->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
      D3D12_HEAP_FLAG_NONE,
      &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
      D3D12_RESOURCE_STATE_GENERIC_READ,
      nullptr,
      IID_PPV_ARGS(&m_vertexBuffer)));

    // Copy the triangle data to the vertex buffer.
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    m_vertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
  }

  // Create synchronization objects and wait until assets have been uploaded to the GPU.
  {
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;

    // Create an event handle to use for frame synchronization.
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
      ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    // Wait for the command list to execute; we are reusing the same command 
    // list in our main loop but for now, we just want to wait for setup to 
    // complete before continuing.
    WaitForPreviousFrame();
  }
}

// Update frame-based values.
void D3D12HelloTriangle::OnUpdate()
{
}

// Render the scene.
void D3D12HelloTriangle::OnRender()
{
  // Record all the commands we need to render the scene into the command list.
  PopulateCommandList();

  // Execute the command list.
  ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };  
  Bunny::Graphics::DX12::Core::g_CommandManager.GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

  // Present the frame.
  ThrowIfFailed(theDisplay_.m_swapChain->Present(1, 0));

  WaitForPreviousFrame();
}

void D3D12HelloTriangle::OnDestroy()
{
  // Ensure that the GPU is no longer referencing resources that are about to be
  // cleaned up by the destructor.
  WaitForPreviousFrame();

  CloseHandle(m_fenceEvent);
}

void D3D12HelloTriangle::PopulateCommandList()
{
  // Command list allocators can only be reset when the associated 
  // command lists have finished execution on the GPU; apps should use 
  // fences to determine GPU execution progress.
  ThrowIfFailed(m_commandAllocator->Reset());

  // However, when ExecuteCommandList() is called on a particular command 
  // list, that command list can then be reset at any time and must be before 
  // re-recording.
  ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

  // Set necessary state.
  m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
  m_commandList->RSSetViewports(1, &theDisplay_.m_viewport);
  m_commandList->RSSetScissorRects(1, &theDisplay_.m_scissorRect);

  // Indicate that the back buffer will be used as a render target.
  m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(theDisplay_.m_renderTargets[theDisplay_.m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(theDisplay_.m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), theDisplay_.m_frameIndex, theDisplay_.m_rtvDescriptorSize);
  m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

  // Record commands.
  const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
  m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
  m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
  m_commandList->DrawInstanced(3, 1, 0, 0);

  // Indicate that the back buffer will now be used to present.
  m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(theDisplay_.m_renderTargets[theDisplay_.m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

  ThrowIfFailed(m_commandList->Close());
}

void D3D12HelloTriangle::WaitForPreviousFrame()
{
  // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
  // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
  // sample illustrates how to use fences for efficient resource usage and to
  // maximize GPU utilization.

  // Signal and increment the fence value.
  const UINT64 fence = m_fenceValue;
  ThrowIfFailed(Bunny::Graphics::DX12::Core::g_CommandManager.GetCommandQueue()->Signal(m_fence.Get(), fence));
  m_fenceValue++;

  // Wait until the previous frame is finished.
  if (m_fence->GetCompletedValue() < fence)
  {
    ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
    WaitForSingleObject(m_fenceEvent, INFINITE);
  }

  theDisplay_.m_frameIndex = theDisplay_.m_swapChain->GetCurrentBackBufferIndex();
}
