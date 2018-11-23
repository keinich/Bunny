#include "RootSignature.h"
#include <map>
#include "Hash.h"
#include "DX12Core.h"

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

        size_t hashCode = Math::HashState(&rootDesc.Flags);
        hashCode = Math::HashState(rootDesc.pStaticSamplers, numSamplers_, hashCode);

        for (UINT param = 0; param < numParameters_; ++param) {
          const D3D12_ROOT_PARAMETER& rootParam = rootDesc.pParameters[param];
          descriptorTableSize_[param] = 0;

          if (rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            ASSERT(rootParam.DescriptorTable.pDescriptorRanges != nullptr);

            hashCode = Math::HashState(
              rootParam.DescriptorTable.pDescriptorRanges, rootParam.DescriptorTable.NumDescriptorRanges, hashCode
            );

            if (rootParam.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
              samplerTableBitMap_ |= (1 << param);
            else
              descriptorTableBitMap_ |= (1 << param);

            for (UINT tableRange = 0; tableRange < rootParam.DescriptorTable.NumDescriptorRanges; ++tableRange)
              descriptorTableSize_[param] += rootParam.DescriptorTable.pDescriptorRanges[tableRange].NumDescriptors;
          }
          else {
            hashCode = Math::HashState(&rootParam, 1, hashCode);
          }
        }

        ID3D12RootSignature** rsRef = nullptr;
        bool firstCompile = false;
        {
          static std::mutex s_HashMapMutex;
          std::lock_guard<std::mutex> CS(s_HashMapMutex);
          auto iter = gs_RootSignatureHashMap.find(hashCode);

          if (iter == gs_RootSignatureHashMap.end()) {
            rsRef = gs_RootSignatureHashMap[hashCode].GetAddressOf();
            firstCompile = true;
          }
          else {
            rsRef = iter->second.GetAddressOf();
          }
        }

        if (firstCompile) {
          Microsoft::WRL::ComPtr<ID3DBlob> pOutBlob, pErrorBlob;

          ASSERT_SUCCEEDED(
            D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf())
          );

          ASSERT_SUCCEEDED(
            Core::g_Device->CreateRootSignature(1, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), IID_PPV_ARGS(&signature_))
          );

          signature_->SetName(name.c_str());

          gs_RootSignatureHashMap[hashCode].Attach(signature_);
          ASSERT(*rsRef == signature_);
        }
        else {
          while (*rsRef == nullptr)
            std::this_thread::yield();
          signature_ = *rsRef;
        }

        finalized_ = TRUE;

      }
    }
  }
}
