#include "DX12Core.h"
#include "CommandListManager.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      namespace Core {
        ID3D12Device* g_Device = nullptr;
        CommandListManager g_CommandManager;
        //uint32_t g_DisplayWidth = 1920; //TODO these are not dx-specific
        //uint32_t g_DisplayHeight = 1080;

      }
    }
  }
}
