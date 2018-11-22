#include "Platform.h"
#include "Win32Application.h"
#include "Helpers.h"

namespace Bunny {
  namespace Platform {
    void InitPlatformApplication(void * app)
    {
#ifdef BN_PLATFORM_WINDOWS
      Win32::Win32Application::Init(app);
#endif
    }
    void RunPlatformApplication()
    {
#ifdef BN_PLATFORM_WINDOWS
      Win32::Win32Application::Run();
#endif
    }

    Win32::Win32Window * GetMainPlatformWindow()
    {
      return Win32::Win32Application::GetMainWindow();
    }
    
  }
}