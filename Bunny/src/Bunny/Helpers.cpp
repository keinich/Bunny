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
    char * ToCharStar(WCHAR* widecharstar)
    {
      _bstr_t b(widecharstar);
      char* r = b;
      return r;
    }
  }
}