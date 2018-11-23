#pragma once
#include "stdafx.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      class RootParameter
      {
      public:
        RootParameter() {
          rootParam_.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
        }

        ~RootParameter() { Clear(); }

        void Clear();

        void InitAsConstants(UINT shaderRegister, UINT numDwords, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        void InitAsConstantBuffer(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        void InitAsBufferSRV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        void InitAsBufferUAV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        void InitAsDescriptorTable(UINT rangeCount, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

        void SetTableRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT shaderRegister, UINT count, UINT space = 0);

        const D3D12_ROOT_PARAMETER& operator() (void) const { return rootParam_; }

      protected:
        D3D12_ROOT_PARAMETER rootParam_;
      };

    }
  }
}
