#pragma once
#include "Win32Window.h"

#ifdef BN_PLATFORM_WINDOWS
#define PLATFORM_WINDOW Win32::Win32Window
#endif

namespace Bunny {
  namespace Graphics {
    class Display;
  }
  namespace Platform {
    void InitPlatformApplication(void* app);
    void RunPlatformApplication(); 
    PLATFORM_WINDOW* GetMainPlatformWindow();
  }
}

