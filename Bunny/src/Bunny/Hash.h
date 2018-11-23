#pragma once

#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif

#include <stdint.h>
#include <intrin.h>
#include "Functions.inl"

namespace Bunny {
  namespace Math {
    inline size_t HashRange(const uint32_t* const begin, const uint32_t* const end, size_t hash) {
      
      const uint64_t* iter64 = (const uint64_t*)AlignUp(begin, 8);
      const uint64_t* const end64 = (const uint64_t* const)AlignDown(end, 8);

      if ((uint32_t*)iter64 > begin)
        hash = _mm_crc32_u32((uint32_t)hash, *begin);

      while (iter64 < end64)
        hash = _mm_crc32_u64((uint64_t)hash, *iter64++);

      if ((uint32_t*)iter64 < end)
        hash = _mm_crc32_u32((uint32_t)hash, *(uint32_t*)iter64);

      return hash;
    }

    template <typename T> inline size_t HashState(const T* stateDesc, size_t count = 1, size_t  hash = 2166136261U) {
      static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
      return HashRange((uint32_t*)stateDesc, (uint32_t*)(stateDesc + count), hash);
    }
  }
}