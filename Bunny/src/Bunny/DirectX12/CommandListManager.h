#pragma once

#include "stdafx.h"
#include "CommandAllocatorPool.h"
#include "CommandQueue.h"

namespace Bunny {
  namespace DirectX12 {


    class CommandListManager {
    public:
      CommandListManager();
      ~CommandListManager();

      void Create(ID3D12Device* pDevice);
      void Shutdown();

      CommandQueue& GetGraphicsQueue(void) { return graphicsQueue_; }
      CommandQueue& GetComputeQueue(void) { return computeQueue_; }
      CommandQueue& GetCopyQueue(void) { return copyQueue_; }

      CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) {
        switch (type)
        {
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: return computeQueue_;
        case D3D12_COMMAND_LIST_TYPE_COPY: return copyQueue_;
        default: return graphicsQueue_;
        }
      }

      //TODO this function should be named something like GetGraphicsQueueCommandQueue..
      ID3D12CommandQueue* GetCommandQueue() {
        return graphicsQueue_.GetCommandQueue();
      }

      void CreateNewCommandList(
        D3D12_COMMAND_LIST_TYPE Type,
        ID3D12GraphicsCommandList** List,
        ID3D12CommandAllocator** Allocator
      );

      bool IsFenceComplete(uint64_t FenceValue) {
        return GetQueue(D3D12_COMMAND_LIST_TYPE(FenceValue >> 56)).IsFenceComplete(FenceValue);
      }

      void WaitForFence(uint64_t FenceValue);

      void IdleGpu(void) {
        graphicsQueue_.WaitForIdle();
        computeQueue_.WaitForIdle();
        copyQueue_.WaitForIdle();
      }

    private:

      ID3D12Device* pDevice_;

      CommandQueue graphicsQueue_;
      CommandQueue computeQueue_;
      CommandQueue copyQueue_;
    };
  }
}
