#include "RootSignature.h"
#include <map>

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> gs_RootSignatureHashMap;

      void RootSignature::DestroyAll(void) {
        gs_RootSignatureHashMap.clear();
      }
      void RootSignature::Reset(UINT numRootParams, UINT numStaticSamplers) {
        if (numRootParams > 0)
          paramArray_.reset(new RootParameter[numRootParams]);
        else
          paramArray_ = nullptr;
        numParameters_ = numRootParams;

        if (numStaticSamplers > 0)
          samplerArray_.reset(new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers]);
        else
          samplerArray_ = nullptr;

        numSamplers_ = numStaticSamplers;
        numInitializedStaticSamplers_ = 0;
      }

      void RootSignature::InitStaticSampler(UINT shaderRegister, const D3D12_SAMPLER_DESC & nonStaticSamplerDesc, D3D12_SHADER_VISIBILITY visibility) {
        ASSERT(numInitializedStaticSamplers_ < numSamplers_);
        D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc = samplerArray_[numInitializedStaticSamplers_++];

        staticSamplerDesc.Filter = nonStaticSamplerDesc.Filter;
        staticSamplerDesc.AddressU = nonStaticSamplerDesc.AddressU;
        staticSamplerDesc.AddressV = nonStaticSamplerDesc.AddressV;
        staticSamplerDesc.AddressW = nonStaticSamplerDesc.AddressW;
        staticSamplerDesc.MipLODBias = nonStaticSamplerDesc.MipLODBias;
        staticSamplerDesc.MaxAnisotropy = nonStaticSamplerDesc.MaxAnisotropy;
        staticSamplerDesc.ComparisonFunc = nonStaticSamplerDesc.ComparisonFunc;
        staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        staticSamplerDesc.MinLOD = nonStaticSamplerDesc.MinLOD;
        staticSamplerDesc.MaxLOD = nonStaticSamplerDesc.MaxLOD;
        staticSamplerDesc.ShaderRegister = shaderRegister;
        staticSamplerDesc.RegisterSpace = 0;
        staticSamplerDesc.ShaderVisibility = visibility;

        if (staticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
          staticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
          staticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
        {
          WARN_ONCE_IF_NOT(
            // Transparent Black
            nonStaticSamplerDesc.BorderColor[0] == 0.0f &&
            nonStaticSamplerDesc.BorderColor[1] == 0.0f &&
            nonStaticSamplerDesc.BorderColor[2] == 0.0f &&
            nonStaticSamplerDesc.BorderColor[3] == 0.0f ||
            // Opaque Black
            nonStaticSamplerDesc.BorderColor[0] == 0.0f &&
            nonStaticSamplerDesc.BorderColor[1] == 0.0f &&
            nonStaticSamplerDesc.BorderColor[2] == 0.0f &&
            nonStaticSamplerDesc.BorderColor[3] == 1.0f ||
            // Opaque White
            nonStaticSamplerDesc.BorderColor[0] == 1.0f &&
            nonStaticSamplerDesc.BorderColor[1] == 1.0f &&
            nonStaticSamplerDesc.BorderColor[2] == 1.0f &&
            nonStaticSamplerDesc.BorderColor[3] == 1.0f,
            "Sampler border color does not match static sampler limitations");

          if (nonStaticSamplerDesc.BorderColor[3] == 1.0f)
          {
            if (nonStaticSamplerDesc.BorderColor[0] == 1.0f)
              staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
            else
              staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
          }
          else
            staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        }

      }

      void RootSignature::Finalize(const std::wstring & name, D3D12_ROOT_SIGNATURE_FLAGS flags) {
        if (finalized_)
          return;

        ASSERT(numInitializedStaticSamplers_ == numSamplers_);
        
        D3D12_ROOT_SIGNATURE_DESC rootDesc;
        rootDesc.NumParameters = numParameters_;
        rootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)paramArray_.get();
        rootDesc.NumStaticSamplers = numSamplers_;
        rootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)samplerArray_.get();
        rootDesc.Flags = flags;

        descriptorTableBitMap_ = 0;
        samplerTableBitMap_ = 0;

        //size_t hashCode = 

      }
    }
  }
}
