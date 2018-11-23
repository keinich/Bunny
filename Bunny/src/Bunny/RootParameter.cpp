#include "RootParameter.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      void RootParameter::Clear() {
        if (rootParam_.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
          delete[] rootParam_.DescriptorTable.pDescriptorRanges;

        rootParam_.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
      }

      void RootParameter::InitAsConstants(UINT shaderRegister, UINT numDwords, D3D12_SHADER_VISIBILITY visibility) {
        rootParam_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        rootParam_.ShaderVisibility = visibility;
        rootParam_.Constants.Num32BitValues = numDwords;
        rootParam_.Constants.ShaderRegister = shaderRegister;
        rootParam_.Constants.RegisterSpace = 0;
      }

      void RootParameter::InitAsConstantBuffer(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility) {
        rootParam_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParam_.ShaderVisibility = visibility;
        rootParam_.Descriptor.ShaderRegister = shaderRegister;
        rootParam_.Descriptor.RegisterSpace = 0;
      }

      void RootParameter::InitAsBufferSRV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility) {
        rootParam_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParam_.ShaderVisibility = visibility;
        rootParam_.Descriptor.ShaderRegister = shaderRegister;
        rootParam_.Descriptor.RegisterSpace = 0;
      }

      void RootParameter::InitAsBufferUAV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility) {
        rootParam_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        rootParam_.ShaderVisibility = visibility;
        rootParam_.Descriptor.ShaderRegister = shaderRegister;
        rootParam_.Descriptor.RegisterSpace = 0;
      }

      void RootParameter::InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, D3D12_SHADER_VISIBILITY visibility) {
        InitAsDescriptorTable(1, visibility);
        SetTableRange(0, type, shaderRegister, count);
      }

      void RootParameter::InitAsDescriptorTable(UINT rangeCount, D3D12_SHADER_VISIBILITY visibility) {
        rootParam_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParam_.ShaderVisibility = visibility;
        rootParam_.DescriptorTable.NumDescriptorRanges = rangeCount;
        rootParam_.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rangeCount];
      }

      void RootParameter::SetTableRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT shaderRegister, UINT count, UINT space) {
        D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(rootParam_.DescriptorTable.pDescriptorRanges + rangeIndex);
        range->RangeType = Type;
        range->NumDescriptors = count;
        range->BaseShaderRegister = shaderRegister;
        range->RegisterSpace = space;
        range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
      }
    }
  }
}