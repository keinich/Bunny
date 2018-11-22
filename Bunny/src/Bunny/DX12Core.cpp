#include "DX12Core.h"
#include "CommandListManager.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      namespace Core {
        ID3D12Device* g_Device = nullptr;
        CommandListManager g_CommandManager;

        DescriptorAllocator g_DescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
        {
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        };

      }
    }
  }
}
