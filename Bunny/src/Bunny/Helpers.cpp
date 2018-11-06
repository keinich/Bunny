#include "Helpers.h"

namespace Bunny {
  namespace Helpers {
    void ThrowIfFailed(HRESULT hr)
    {
      if (FAILED(hr))
      {
        throw HrException(hr);
      }
    }
  }
}