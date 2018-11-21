#pragma once
#include "CommandAllocatorPool.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {
      class CommandQueue {
        friend class CommandListManager;
      public:
        CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
        ~CommandQueue();

        void Create(ID3D12Device* pDevice);
        void Shutdown();

        inline bool IsReady() { return commandQueue_ != nullptr; }

        uint64_t IncrementFence(void);
        bool IsFenceComplete(uint64_t FenceValue);
        void StallForFence(uint64_t FenceValue);
        void StallForProducer(CommandQueue& Producer);
        void WaitForFence(uint64_t FenceValue);
        void WaitForIdle(void) { WaitForFence(IncrementFence()); }

        ID3D12CommandQueue* GetCommandQueue() { return commandQueue_; }

        uint64_t GetNextFenceValue() { return nextFenceValue_; }

      private:

        uint64_t ExecuteCommandList(ID3D12CommandList* List);
        ID3D12CommandAllocator* RequestAllocator(void);
        void DiscardAllocator(uint64_t FenceValueForReset, ID3D12CommandAllocator* Allocator);

        ID3D12CommandQueue* commandQueue_;

        const D3D12_COMMAND_LIST_TYPE type_;

        CommandAllocatorPool allocatorPool_;
        std::mutex fenceMutex_;
        std::mutex eventMutex_;

        ID3D12Fence* pFence_;
        uint64_t nextFenceValue_;
        uint64_t lastCompletedFenceValue_;
        HANDLE fenceEventHandle_;
      };
    }
  }
}