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

#pragma once

#include "DXSample.h"
#include "Display.h"
#include "RootSignature.h"

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

class D3D12HelloTriangle : public DXSample
{
public:
  D3D12HelloTriangle(UINT width, UINT height, std::wstring name);

  virtual void OnInit();
  virtual void OnUpdate();
  virtual void OnRender();
  virtual void OnDestroy();

private:
  static const UINT FrameCount = 2;

  struct Vertex
  {
    XMFLOAT3 position;
    XMFLOAT4 color;
  };

  Bunny::Graphics::DX12::Display theDisplay_;

  // Pipeline objects. 
  //ComPtr<ID3D12Device> m_device;
  ComPtr<ID3D12CommandAllocator> m_commandAllocator;  
  Bunny::Graphics::DX12::RootSignature rootSignature_;

  ComPtr<ID3D12PipelineState> m_pipelineState;
  ComPtr<ID3D12GraphicsCommandList> m_commandList;

  // App resources.
  ComPtr<ID3D12Resource> m_vertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

  // Synchronization objects.
  HANDLE m_fenceEvent;
  ComPtr<ID3D12Fence> m_fence;
  UINT64 m_fenceValue;

  bool m_bTypedUAVLoadSupport_R11G11B10_FLOAT;

  void LoadPipeline();
  void CheckTypedLoadUavSupport();
  void CreateInfoQueue();
  void LoadAssets();
  void PopulateCommandList();
  void WaitForPreviousFrame();
};