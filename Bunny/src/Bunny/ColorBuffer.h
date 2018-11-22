#pragma once
#include "PixelBuffer.h"
#include "Color.h"
#include "Helpers.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      class ColorBuffer :
        public PixelBuffer
      {
      public:
        ColorBuffer(Color ClearColor = Color(0.0f, 0.0f, 0.0f, 0.0f))
          : clearColor_(ClearColor), numMipMaps_(0), fragmentCount_(1), sampleCount_(1)
        {
          srvHanlde_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
          rtvHanlde_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
          std::memset(uavHanlde_, 0xFF, sizeof(uavHanlde_));
        }

        void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource);

        void Create(
          const std::wstring& name, 
          uint32_t width, uint32_t height, 
          uint32_t numMps,
          DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
        );

        void CreateArray(
          const std::string_view& name, 
          uint32_t width, uint32_t height, 
          uint32_t arrayCount, DXGI_FORMAT format, 
          D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
        );

        const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV(void) const { return srvHanlde_; }
        const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV(void) const { return rtvHanlde_; }
        const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV(void) const { return uavHanlde_[0]; }

        Color GetClearColor(void) const { return clearColor_; }
        void SetClearColor(Color clearColor) { clearColor_ = clearColor; }

        void SetMsaaMode(uint32_t numColorSamples, uint32_t numCoverageSamples)
        {
          ASSERT(numCoverageSamples >= numColorSamples);
          fragmentCount_ = numColorSamples;
          sampleCount_ = numCoverageSamples;
        }

        // This will work for all texture sizes, but it's recommended for speed and quality
        // that you use dimensions with powers of two (but not necessarily square.)  Pass
        // 0 for ArrayCount to reserve space for mips at creation time.
        void GenerateMipMaps(CommandContext& context);

      protected:

        D3D12_RESOURCE_FLAGS CombineResourceFlags(void) const
        {
          D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;

          if (Flags == D3D12_RESOURCE_FLAG_NONE && fragmentCount_ == 1)
            Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

          return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | Flags;
        }

        static inline uint32_t ComputeNumMips(uint32_t width, uint32_t height)
        {
          uint32_t HighBit;
          _BitScanReverse((unsigned long*)&HighBit, width | height);
          return HighBit + 1;
        }

        void CreateDerivedViews(ID3D12Device* pDevice, DXGI_FORMAT format, uint32_t arraySize, uint32_t numMips = 1);

        Color clearColor_;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHanlde_;
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHanlde_;
        D3D12_CPU_DESCRIPTOR_HANDLE uavHanlde_[12];
        uint32_t numMipMaps_;
        uint32_t fragmentCount_;
        uint32_t sampleCount_;
        
      };

    }
  }
}
