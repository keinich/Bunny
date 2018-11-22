#pragma once
#include "stdafx.h"

namespace Bunny {
  namespace Graphics {

    class Display
    {
    public:
      Display();
      ~Display();

      inline int32_t GetDisplayWidth() { return displayWidth_; }
      inline int32_t GetDisplayHeight() { return displayHeight_; }

    private:
      int32_t displayWidth_;
      int32_t displayHeight_;
    };

  }
}
