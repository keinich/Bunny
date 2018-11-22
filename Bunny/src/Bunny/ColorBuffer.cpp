#include "ColorBuffer.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      void ColorBuffer::CreateFromSwapChain(const std::wstring & name, ID3D12Resource * baseResource) {
        AssociateWithResource(name, baseResource, D3D12_RESOURCE_STATE_PRESENT);

        //rtvHanlde_ = 
      }
    }
  }
}
