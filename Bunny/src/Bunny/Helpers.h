#pragma once

//Windows Includes
#include <windows.h>
#include <wrl.h>
#include <winerror.h>

#include "DirectX12/d3dx12.h"

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

  }
}