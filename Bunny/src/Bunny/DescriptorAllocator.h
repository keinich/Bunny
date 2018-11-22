#pragma once
#include "stdafx.h"
#include <mutex>

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      class DescriptorAllocator {
      public:
        DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) :
          type_(type),
          currentHeap_(nullptr) {}
        
        D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t count);

        static void DestroyAll();

      protected:
        static const uint32_t s_NumDescriptorsPerHeap = 256;
        static std::mutex s_AllocationMutex;
        static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> s_DescriptorHeapPool;
        static ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

        D3D12_DESCRIPTOR_HEAP_TYPE type_;
        ID3D12DescriptorHeap* currentHeap_;
        D3D12_CPU_DESCRIPTOR_HANDLE currentHandle_;
        uint32_t descriptorSize_;
        uint32_t remainingFreeHandles_;
      };

    }
  }
}
