#include "CommandQueue.h"
#include "../Helpers.h"

namespace Bunny {

  CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE Type) :
    m_Type(Type),
    m_CommandQueue(nullptr),
    m_pFence(nullptr),
    m_NextFenceValue((uint64_t)Type << 56 | 1),
    m_LastCompletedFenceValue((uint64_t)Type << 56),
    m_AllocatorPool(Type)
  {
  }


  CommandQueue::~CommandQueue()
  {
    Shutdown();
  }

  void CommandQueue::Shutdown()
  {
    if (m_CommandQueue == nullptr)
      return;

    m_AllocatorPool.Shutdown();

    CloseHandle(m_FenceEventHandle);

    m_pFence->Release();
    m_pFence = nullptr;

    m_CommandQueue->Release();
    m_CommandQueue = nullptr;
  }

  void CommandQueue::Create(ID3D12Device* pDevice)
  {
    if (pDevice == nullptr)
      throw;

    if (IsReady()) //TODO Assert Utilitiies
      throw;

    if (m_AllocatorPool.Size() != 0)
      throw;

    D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
    QueueDesc.Type = m_Type;
    QueueDesc.NodeMask = 1;
    pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_CommandQueue));
    m_CommandQueue->SetName(L"CommandListManager::m_CommandQueue");

    Helpers::ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
    m_pFence->SetName(L"CommandListManager::m_pFence");
    m_pFence->Signal((uint64_t)m_Type << 56);

    m_FenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
    if (m_FenceEventHandle == INVALID_HANDLE_VALUE)
      throw;

    m_AllocatorPool.Create(pDevice);

    if (!IsReady())
      throw;

  }

  uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* List)
  {
    std::lock_guard<std::mutex> LockGuard(m_FenceMutex);

    Helpers::ThrowIfFailed(((ID3D12GraphicsCommandList*)List)->Close());

    m_CommandQueue->ExecuteCommandLists(1, &List);

    m_CommandQueue->Signal(m_pFence, m_NextFenceValue);
    return m_NextFenceValue++;
  }

  uint64_t CommandQueue::IncrementFence(void)
  {
    std::lock_guard<std::mutex> LockGuard(m_FenceMutex);

    m_CommandQueue->Signal(m_pFence, m_NextFenceValue);
    return m_NextFenceValue++;
  }

  bool CommandQueue::IsFenceComplete(uint64_t FenceValue)
  {
    if (FenceValue > m_LastCompletedFenceValue)
    {
      uint64_t cfv = m_pFence->GetCompletedValue();
      if (cfv > m_LastCompletedFenceValue)
        m_LastCompletedFenceValue = cfv;
      //m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_pFence->GetCompletedValue());
      // TODO min max Utilities
    }

    return FenceValue <= m_LastCompletedFenceValue;
  }
}
