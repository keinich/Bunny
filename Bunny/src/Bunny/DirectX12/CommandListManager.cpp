#include "CommandListManager.h"
#include "../Helpers.h"

namespace Bunny {
  namespace DirectX12 {
    
    namespace DX12Core
    {
      extern CommandListManager g_CommandManager;
    }

    CommandListManager::CommandListManager() :
      pDevice_(nullptr),
      graphicsQueue_(D3D12_COMMAND_LIST_TYPE_DIRECT),
      computeQueue_(D3D12_COMMAND_LIST_TYPE_COMPUTE),
      copyQueue_(D3D12_COMMAND_LIST_TYPE_COPY) {
    }


    CommandListManager::~CommandListManager() {
      Shutdown();
    }

    void CommandListManager::Create(ID3D12Device* pDevice) {
      ASSERT(pDevice != nullptr);

      pDevice_ = pDevice;

      graphicsQueue_.Create(pDevice);
      computeQueue_.Create(pDevice);
      copyQueue_.Create(pDevice);
    }

    void CommandListManager::Shutdown() {
      graphicsQueue_.Shutdown();
      computeQueue_.Shutdown();
      copyQueue_.Shutdown();
    }

    void CommandListManager::CreateNewCommandList(
      D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator
    ) {
      ASSERT(type != D3D12_COMMAND_LIST_TYPE_BUNDLE, "Bundles are not supported!");
      switch (type)
      {
      case D3D12_COMMAND_LIST_TYPE_DIRECT: *allocator = graphicsQueue_.RequestAllocator();
        break;
      case D3D12_COMMAND_LIST_TYPE_BUNDLE: break;
      case D3D12_COMMAND_LIST_TYPE_COMPUTE: *allocator = computeQueue_.RequestAllocator();
        break;
      case D3D12_COMMAND_LIST_TYPE_COPY: *allocator = copyQueue_.RequestAllocator();
        break;
      }

      ASSERT_SUCCEEDED(pDevice_->CreateCommandList(1, type, *allocator, nullptr, IID_PPV_ARGS(list)));
      (*list)->SetName(L"CommandList");
    }

    void CommandListManager::WaitForFence(uint64_t FenceValue)
    {
      CommandQueue& Producer = DX12Core::g_CommandManager.GetQueue((D3D12_COMMAND_LIST_TYPE)(FenceValue >> 56)); //TODO why is this nethod not static?
      Producer.WaitForFence(FenceValue);
    }
  }
}