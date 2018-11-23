#pragma once
#include "stdafx.h"
#include "RootParameter.h"
#include "Helpers.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {
      
      class RootSignature
      {
      public:
        RootSignature(UINT numRootParams = 0, UINT numStaticSamplers = 0) : finalized_(FALSE), numParameters_(numRootParams) {
          Reset(numRootParams, numStaticSamplers);
        }

        ~RootSignature() {}

        static void DestroyAll(void);

        void Reset(UINT numRootParams, UINT numStaticSamplers = 0);

        RootParameter& operator[] (size_t entryIndex){
          ASSERT(entryIndex < numParameters_);
        return paramArray_.get()[entryIndex];
        }

        const RootParameter& operator[] (size_t entryIndex) const {
          ASSERT(entryIndex < numParameters_);
          return paramArray_.get()[entryIndex];
        }

        void InitStaticSampler(
          UINT shaderRegister, const D3D12_SAMPLER_DESC& nonStaticSamplerDesc,
          D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL
        );

        void Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

        ID3D12RootSignature* GetSignature() const { return signature_; }

      protected:
        bool finalized_;
        UINT numParameters_;
        UINT numSamplers_;
        UINT numInitializedStaticSamplers_;
        uint32_t descriptorTableBitMap_;
        uint32_t samplerTableBitMap_;
        uint32_t descriptorTableSize_[16];
        std::unique_ptr<RootParameter[]> paramArray_;
        std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> samplerArray_;
        ID3D12RootSignature* signature_;
      };

    }
  }
}
