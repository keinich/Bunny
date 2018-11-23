namespace Bunny {
  namespace Math {
    inline float Max(float a, float b) { return a > b ? a : b; }
    inline float Min(float a, float b) { return a < b ? a : b; }
    inline float Clamp(float v, float a, float b) { return Min(Max(v, a), b); }

    template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask) {
      return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask) {
      return (T)(((size_t)value) & ~mask);
    }

    template <typename T> __forceinline T AlignUp(T value, size_t alignment) {
      return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T> __forceinline T AlignDown(T value, size_t alignment) {
      return AlignDownWithMask(value, alignment - 1);
    }

  }
}