#include "CommandQueue.h"
#include "CommandListManager.h"
#include "Helpers.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      namespace Core
      {
        extern CommandListManager g_CommandManager;
      }

      CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE Type) :
        type_(Type),
        commandQueue_(nullptr),
        pFence_(nullptr),
        nextFenceValue_((uint64_t)Type << 56 | 1),
        lastCompletedFenceValue_((uint64_t)Type << 56),
        allocatorPool_(Type) {
      }


      CommandQueue::~CommandQueue() {
        Shutdown();
      }

      void CommandQueue::Shutdown() {
        if (commandQueue_ == nullptr)
          return;

        allocatorPool_.Shutdown();

        CloseHandle(fenceEventHandle_);

        pFence_->Release();
        pFence_ = nullptr;

        commandQueue_->Release();
        commandQueue_ = nullptr;
      }

      void CommandQueue::Create(ID3D12Device* pDevice)
      {
        if (pDevice == nullptr)
          throw;

        if (IsReady()) //TODO Assert Utilitiies
          throw;

        if (allocatorPool_.Size() != 0)
          throw;

        D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
        QueueDesc.Type = type_;
        QueueDesc.NodeMask = 1;
        pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&commandQueue_));
        commandQueue_->SetName(L"CommandListManager::m_CommandQueue");

        Helpers::ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence_)));
        pFence_->SetName(L"CommandListManager::m_pFence");
        pFence_->Signal((uint64_t)type_ << 56);

        fenceEventHandle_ = CreateEvent(nullptr, false, false, nullptr);
        if (fenceEventHandle_ == INVALID_HANDLE_VALUE)
          throw;

        allocatorPool_.Create(pDevice);

        if (!IsReady())
          throw;

      }

      uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* List)
      {
        std::lock_guard<std::mutex> LockGuard(fenceMutex_);

        Helpers::ThrowIfFailed(((ID3D12GraphicsCommandList*)List)->Close());

        commandQueue_->ExecuteCommandLists(1, &List);

        commandQueue_->Signal(pFence_, nextFenceValue_);
        return nextFenceValue_++;
      }

      uint64_t CommandQueue::IncrementFence(void)
      {
        std::lock_guard<std::mutex> LockGuard(fenceMutex_);

        commandQueue_->Signal(pFence_, nextFenceValue_);
        return nextFenceValue_++;
      }

      bool CommandQueue::IsFenceComplete(uint64_t FenceValue)
      {
        if (FenceValue > lastCompletedFenceValue_)
        {
          uint64_t cfv = pFence_->GetCompletedValue();
          if (cfv > lastCompletedFenceValue_)
            lastCompletedFenceValue_ = cfv;
          //m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_pFence->GetCompletedValue());
          // TODO min max Utilities
        }

        return FenceValue <= lastCompletedFenceValue_;
      }

      void CommandQueue::StallForFence(uint64_t FenceValue)
      {
        CommandQueue& Producer = Core::g_CommandManager.GetQueue((D3D12_COMMAND_LIST_TYPE)(FenceValue >> 56));
        commandQueue_->Wait(Producer.pFence_, FenceValue);
      }

      void CommandQueue::StallForProducer(CommandQueue& Producer)
      {
        ASSERT(Producer.nextFenceValue_ > 0);
        commandQueue_->Wait(Producer.pFence_, Producer.nextFenceValue_ - 1);
      }

      void CommandQueue::WaitForFence(uint64_t FenceValue)
      {
        if (IsFenceComplete(FenceValue))
          return;

        {
          std::lock_guard<std::mutex> LockGuard(eventMutex_);

          pFence_->SetEventOnCompletion(FenceValue, fenceEventHandle_);
          WaitForSingleObject(fenceEventHandle_, INFINITE);
          lastCompletedFenceValue_ = FenceValue;
        }
      }

      ID3D12CommandAllocator* CommandQueue::RequestAllocator()
      {
        uint64_t CompletedFence = pFence_->GetCompletedValue();
        return allocatorPool_.RequestAllocator(CompletedFence);
      }

      void CommandQueue::DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator)
      {
        allocatorPool_.DiscardAllocator(FenceValue, Allocator);
      }
    }
  }
}