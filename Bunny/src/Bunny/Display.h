#pragma once
#include "stdafx.h"
#include <wrl.h>
#include "Platform.h"
#include "ColorBuffer.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      class Display
      {
      public:
        Display(UINT width, UINT height);
        ~Display();

        void Init(Microsoft::WRL::ComPtr<IDXGIFactory4> &factory, Microsoft::WRL::ComPtr<ID3D12Device> &device, Platform::PLATFORM_WINDOW* window);

        inline int32_t GetWidth() { return width_; }
        inline int32_t GetHeight() { return height_; }

        //private: TODO make private again!
        UINT m_frameIndex;
        static const UINT FrameCount = 2;
        int32_t width_;
        int32_t height_;

        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
        UINT m_rtvDescriptorSize;

        ColorBuffer displayPlanes_[2];
      };

    }
  }
}
