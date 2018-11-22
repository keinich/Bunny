#include "ColorBuffer.h"
#include "DX12Core.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      void ColorBuffer::CreateFromSwapChain(const std::wstring & name, ID3D12Resource * baseResource) {
        AssociateWithResource(name, baseResource, D3D12_RESOURCE_STATE_PRESENT);

        rtvHanlde_ = Core::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        Core::g_Device->CreateRenderTargetView(pResource_.Get(), nullptr, rtvHanlde_);
      }

      void ColorBuffer::Create(
        const std::wstring & name, 
        uint32_t width, 
        uint32_t height, 
        uint32_t numMps, 
        DXGI_FORMAT format, 
        D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr
      ) {
        numMps = (numMps == 0 ? ComputeNumMips(width, height) : numMps);
        D3D12_RESOURCE_FLAGS flags = CombineResourceFlags();
        D3D12_RESOURCE_DESC resourceDesc = DescribeTex2D(width, height, 1, numMps, format, flags);

        resourceDesc.SampleDesc.Count = fragmentCount_;
        resourceDesc.SampleDesc.Quality = 0;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = format;
        clearValue.Color[0] = clearColor_.R();
        clearValue.Color[1] = clearColor_.G();
        clearValue.Color[2] = clearColor_.B();
        clearValue.Color[3] = clearColor_.A();

        CreateTextureResource(Core::g_Device, name, resourceDesc, clearValue, vidMemPtr);
        CreateDerivedViews(Core::g_Device, format, 1, numMps);
      }

      void ColorBuffer::GenerateMipMaps(CommandContext & context) {
      }

      void ColorBuffer::CreateDerivedViews(ID3D12Device * pDevice, DXGI_FORMAT format, uint32_t arraySize, uint32_t numMips) {
        ASSERT(arraySize == 1 || numMips == 1, "We don't support auto-mips on texture arrays");

        numMipMaps_ = numMips;

        D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
        D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
        D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

        RTVDesc.Format = format;
        UAVDesc.Format = GetUAVFormat(format);
        SRVDesc.Format = format;
        SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (arraySize > 1) {
          RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
          RTVDesc.Texture2DArray.MipSlice = 0;
          RTVDesc.Texture2DArray.FirstArraySlice = 0;
          RTVDesc.Texture2DArray.ArraySize = (UINT)arraySize;

          UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
          UAVDesc.Texture2DArray.MipSlice = 0;
          UAVDesc.Texture2DArray.FirstArraySlice = 0;
          UAVDesc.Texture2DArray.ArraySize = (UINT)arraySize;

          SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
          SRVDesc.Texture2DArray.MipLevels = numMips;
          SRVDesc.Texture2DArray.MostDetailedMip = 0;
          SRVDesc.Texture2DArray.FirstArraySlice = 0;
          SRVDesc.Texture2DArray.ArraySize = (UINT)arraySize;
        }
        else if (fragmentCount_ > 1)
        {
          RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
          SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        }
        else
        {
          RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
          RTVDesc.Texture2D.MipSlice = 0;

          UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
          UAVDesc.Texture2D.MipSlice = 0;

          SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
          SRVDesc.Texture2D.MipLevels = numMips;
          SRVDesc.Texture2D.MostDetailedMip = 0;
        }
        if (srvHanlde_.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
        {
          rtvHanlde_ = Core::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
          srvHanlde_ = Core::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        ID3D12Resource* Resource = pResource_.Get();

        // Create the render target view
        pDevice->CreateRenderTargetView(Resource, &RTVDesc, rtvHanlde_);

        // Create the shader resource view
        pDevice->CreateShaderResourceView(Resource, &SRVDesc, srvHanlde_);

        if (fragmentCount_ > 1)
          return;

        // Create the UAVs for each mip level (RWTexture2D)
        for (uint32_t i = 0; i < numMips; ++i)
        {
          if (uavHanlde_[i].ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
            uavHanlde_[i] = Core::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

          pDevice->CreateUnorderedAccessView(Resource, nullptr, &UAVDesc, uavHanlde_[i]);

          UAVDesc.Texture2D.MipSlice++;
        }
      }

    }
  }
}
