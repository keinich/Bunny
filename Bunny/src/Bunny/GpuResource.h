#pragma once

#include "stdafx.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      class GpuResource
      {
      public:
        GpuResource() :
          gpuVirtualAddress_(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
          userAllocatedMemory_(nullptr),
          usageState_(D3D12_RESOURCE_STATE_COMMON),
          transitionState_((D3D12_RESOURCE_STATES)-1) {}

        GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
          gpuVirtualAddress_(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
          userAllocatedMemory_(nullptr),
          pResource_(pResource),
          usageState_(CurrentState),
          transitionState_((D3D12_RESOURCE_STATES)-1) {}

        virtual void Destroy() {
          pResource_ = nullptr;
          gpuVirtualAddress_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
          if (userAllocatedMemory_ != nullptr) {
            VirtualFree(userAllocatedMemory_, 0, MEM_RELEASE);
            userAllocatedMemory_ = nullptr;
          }
        }

        ID3D12Resource* operator->() { return pResource_.Get(); }
        const ID3D12Resource* operator->() const { return pResource_.Get(); }

        ID3D12Resource* GetResource() { return pResource_.Get(); }
        const ID3D12Resource* GetResource() const { return pResource_.Get(); }

        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return gpuVirtualAddress_; }

      protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> pResource_;
        D3D12_RESOURCE_STATES usageState_;
        D3D12_RESOURCE_STATES transitionState_;
        D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress_;

        void* userAllocatedMemory_;
      };

    }
  }
}
