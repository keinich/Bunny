#include "DescriptorAllocator.h"
#include "DX12Core.h"
#include "Helpers.h"

std::mutex Bunny::Graphics::DX12::DescriptorAllocator::s_AllocationMutex;
std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> Bunny::Graphics::DX12::DescriptorAllocator::s_DescriptorHeapPool;

D3D12_CPU_DESCRIPTOR_HANDLE Bunny::Graphics::DX12::DescriptorAllocator::Allocate(uint32_t count)
{
  if (currentHeap_ == nullptr || remainingFreeHandles_ < count) {
    currentHeap_ = RequestNewHeap(type_);
    currentHandle_ = currentHeap_->GetCPUDescriptorHandleForHeapStart();
    remainingFreeHandles_ = s_NumDescriptorsPerHeap;

    if (descriptorSize_ == 0) {
      descriptorSize_ = Bunny::Graphics::DX12::Core::g_Device->GetDescriptorHandleIncrementSize(type_);
    }
  }

  D3D12_CPU_DESCRIPTOR_HANDLE ret = currentHandle_;
  currentHandle_.ptr += count * descriptorSize_;
  remainingFreeHandles_ -= count;
  return ret;
}

void Bunny::Graphics::DX12::DescriptorAllocator::DestroyAll() {
  s_DescriptorHeapPool.clear();
}

ID3D12DescriptorHeap * Bunny::Graphics::DX12::DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) {
  std::lock_guard<std::mutex> LockGuard(s_AllocationMutex);

  D3D12_DESCRIPTOR_HEAP_DESC desc;
  desc.Type = type;
  desc.NumDescriptors = s_NumDescriptorsPerHeap;
  desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  desc.NodeMask = 1;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;
  ASSERT_SUCCEEDED(Bunny::Graphics::DX12::Core::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pHeap)));
  s_DescriptorHeapPool.emplace_back(pHeap);
  return pHeap.Get();
  
}
