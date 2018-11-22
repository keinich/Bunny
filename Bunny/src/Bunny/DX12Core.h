#pragma once
#include "stdafx.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      //Forward declarations
      class CommandListManager;

      namespace Core {
        //Globals for now
        extern ID3D12Device* g_Device;
        extern CommandListManager g_CommandManager;
      };
    }
  }
}
