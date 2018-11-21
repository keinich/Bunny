#pragma once

#include <comdef.h>

//Windows Includes
#include <windows.h>
#include <wrl.h>
#include <winerror.h>

#include "DirectX12/d3dx12.h"
#include "Log.h"

namespace Bunny {
  namespace Helpers {

    inline std::string HrToString(HRESULT hr)
    {
      char s_str[64] = {};
      sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
      return std::string(s_str);
    }

    class HrException : public std::runtime_error
    {
    public:
      HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
      HRESULT Error() const { return m_hr; }
    private:
      const HRESULT m_hr;
    };

    void ThrowIfFailed(HRESULT hr);

    char* ToCharStar(WCHAR* widecharstar);
    
  }
}

#ifdef RELEASE

#define ASSERT( isTrue, ... ) (void)(isTrue)
#define WARN_ONCE_IF( isTrue, ... ) (void)(isTrue)
#define WARN_ONCE_IF_NOT( isTrue, ... ) (void)(isTrue)
#define DEBUGPRINT( msg, ... ) do {} while(0)

#else    // !RELEASE

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#define ASSERT( isFalse, ... ) \
        if (!(bool)(isFalse)) { \
            BN_CORE_FATAL("test"); \
            __debugbreak(); \
        }

#define ASSERT_SUCCEEDED( hr, ... ) \
        if (FAILED(hr)) { \
            BN_CORE_FATAL("test"); \
            __debugbreak(); \
        }


#define WARN_ONCE_IF( isTrue, ... ) \
    { \
        static bool s_TriggeredWarning = false; \
        if ((bool)(isTrue) && !s_TriggeredWarning) { \
            s_TriggeredWarning = true; \
            BN_CORE_WARN(__VA_ARGS__); \
        } \
    }

#define WARN_ONCE_IF_NOT( isTrue, ... ) WARN_ONCE_IF(!(isTrue), __VA_ARGS__)

#endif