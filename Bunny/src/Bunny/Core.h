#pragma once

#ifdef BN_PLATFORM_WINDOWS

  #ifdef BN_BUILD_DLL
    #define BUNNY_API __declspec(dllexport)
  #else
    #define BUNNY_API __declspec(dllimport)
  #endif
#else
  #error Bunny only supports Windows!
#endif