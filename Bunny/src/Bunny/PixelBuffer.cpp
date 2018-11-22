#include "PixelBuffer.h"
#include "Helpers.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {
      DXGI_FORMAT PixelBuffer::GetBaseFormat(DXGI_FORMAT defaultFormat)
      {
        switch (defaultFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
          return DXGI_FORMAT_R8G8B8A8_TYPELESS;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
          return DXGI_FORMAT_B8G8R8A8_TYPELESS;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
          return DXGI_FORMAT_B8G8R8X8_TYPELESS;

          // 32-bit Z w/ Stencil
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
          return DXGI_FORMAT_R32G8X24_TYPELESS;

          // No Stencil
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
          return DXGI_FORMAT_R32_TYPELESS;

          // 24-bit Z
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
          return DXGI_FORMAT_R24G8_TYPELESS;

          // 16-bit Z w/o Stencil
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
          return DXGI_FORMAT_R16_TYPELESS;

        default:
          return defaultFormat;
        }
      }
      DXGI_FORMAT PixelBuffer::GetUAVFormat(DXGI_FORMAT defaultFormat)
      {
        switch (defaultFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
          return DXGI_FORMAT_R8G8B8A8_UNORM;

        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
          return DXGI_FORMAT_B8G8R8A8_UNORM;

        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
          return DXGI_FORMAT_B8G8R8X8_UNORM;

        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_R32_FLOAT:
          return DXGI_FORMAT_R32_FLOAT;

#ifdef _DEBUG
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_D16_UNORM:

          ASSERT(false, "Requested a UAV format for a depth stencil format.");
#endif

        default:
          return defaultFormat;
        }
      }
      DXGI_FORMAT PixelBuffer::GetDSVFormat(DXGI_FORMAT defaultFormat)
      {
        switch (defaultFormat)
        {
          // 32-bit Z w/ Stencil
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
          return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

          // No Stencil
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
          return DXGI_FORMAT_D32_FLOAT;

          // 24-bit Z
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
          return DXGI_FORMAT_D24_UNORM_S8_UINT;

          // 16-bit Z w/o Stencil
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
          return DXGI_FORMAT_D16_UNORM;

        default:
          return defaultFormat;
        }
      }
      DXGI_FORMAT PixelBuffer::GetDepthFormat(DXGI_FORMAT defaultFormat)
      {
        switch (defaultFormat)
        {
          // 32-bit Z w/ Stencil
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
          return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

          // No Stencil
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
          return DXGI_FORMAT_R32_FLOAT;

          // 24-bit Z
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
          return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

          // 16-bit Z w/o Stencil
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
          return DXGI_FORMAT_R16_UNORM;

        default:
          return DXGI_FORMAT_UNKNOWN;
        }
      }
      DXGI_FORMAT PixelBuffer::GetStencilFormat(DXGI_FORMAT defaultFormat)
      {
        switch (defaultFormat)
        {
          // 32-bit Z w/ Stencil
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
          return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

          // 24-bit Z
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
          return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

        default:
          return DXGI_FORMAT_UNKNOWN;
        }
      }
      size_t PixelBuffer::BytesPerPixel(DXGI_FORMAT Format)
      {
        switch (Format)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
          return 16;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
          return 12;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
          return 8;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
          return 4;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
          return 2;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_P8:
          return 1;

        default:
          return 0;
        }
      }

      void PixelBuffer::AssociateWithResource(const std::wstring& name, ID3D12Resource* pResource, D3D12_RESOURCE_STATES currentState) {
        ASSERT(pResource != nullptr);
        D3D12_RESOURCE_DESC resourceDesc = pResource->GetDesc();

        pResource_.Attach(pResource);
        usageState_ = currentState;

        width_ = (uint32_t)resourceDesc.Width;
        height_ = resourceDesc.Height;
        arraySize_ = resourceDesc.DepthOrArraySize;
        format_ = resourceDesc.Format;

#ifndef RELEASE
        pResource_->SetName(name.c_str());
#else
        (Name);
#endif
      }

      D3D12_RESOURCE_DESC PixelBuffer::DescribeTex2D(
        uint32_t width, uint32_t height, uint32_t DepthOrArraySize, uint32_t NumMips, DXGI_FORMAT format, UINT flags
      ) {
        width_ = width;
        height_ = height;
        arraySize_ = DepthOrArraySize;
        format_ = format;

        D3D12_RESOURCE_DESC Desc = {};
        Desc.Alignment = 0;
        Desc.DepthOrArraySize = (UINT16)DepthOrArraySize;
        Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        Desc.Flags = (D3D12_RESOURCE_FLAGS)flags;
        Desc.Format = GetBaseFormat(format);
        Desc.Height = (UINT)height;
        Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        Desc.MipLevels = (UINT16)NumMips;
        Desc.SampleDesc.Count = 1;
        Desc.SampleDesc.Quality = 0;
        Desc.Width = (UINT64)width;
        return Desc;
      }

      void PixelBuffer::CreateTextureResource(
        ID3D12Device* pDevice, 
        const std::wstring& name,
        const D3D12_RESOURCE_DESC& resourceDesc, 
        D3D12_CLEAR_VALUE clearValue, 
        D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr
      ) {
        Destroy();

        CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);
        ASSERT_SUCCEEDED(pDevice->CreateCommittedResource(
          &HeapProps, D3D12_HEAP_FLAG_NONE,
          &resourceDesc, D3D12_RESOURCE_STATE_COMMON,
          &clearValue, 
          IID_PPV_ARGS(&pResource_)
        ));

#ifndef RELEASE
        pResource_->SetName(name.c_str());
#else
        (name);
#endif
      }
    }
  }
}