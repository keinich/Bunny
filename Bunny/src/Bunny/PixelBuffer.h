#pragma once
#include "GpuResource.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      class PixelBuffer : public GpuResource
      {
      public:
        PixelBuffer() : width_(0), height_(0), arraySize_(0),format_(DXGI_FORMAT_UNKNOWN) {}

        uint32_t GetWidth(void) const { return width_; }
        uint32_t GetHeight(void) const { return height_; }
        uint32_t GetDepth(void) const { return arraySize_; }
        const DXGI_FORMAT& GetFormat(void) const { return format_; }

      protected:

        D3D12_RESOURCE_DESC DescribeTex2D(
          uint32_t width, uint32_t height, uint32_t DepthOrArraySize, uint32_t NumMips, DXGI_FORMAT format, UINT flags
        );

        void AssociateWithResource(
          ID3D12Device* pDevice, const std::wstring& name, ID3D12Resource* pResource, D3D12_RESOURCE_STATES currentState
        );

        void CreateTextureResource(
          ID3D12Device* pDevice,
          const std::wstring& name,
          const D3D12_RESOURCE_DESC& resourceDesc,
          D3D12_CLEAR_VALUE clearValue,
          D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
        );

        static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT Format);
        static size_t BytesPerPixel(DXGI_FORMAT Format);

        uint32_t width_;
        uint32_t height_;
        uint32_t arraySize_;
        DXGI_FORMAT format_;
      };

    }
  }
}
