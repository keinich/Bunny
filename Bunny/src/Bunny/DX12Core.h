#pragma once
#include "stdafx.h"
#include "DescriptorAllocator.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      //Forward declarations
      class CommandListManager;

      namespace Core {

        void Init(void);

        //Globals for now
        extern ID3D12Device* g_Device;
        extern CommandListManager g_CommandManager;
        extern DescriptorAllocator g_DescriptorAllocator[];

        inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count = 1) {
          return g_DescriptorAllocator[type].Allocate(count);
        }

      };
    }
  }
}
