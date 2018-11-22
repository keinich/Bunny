namespace Bunny {
  namespace Math {
    inline float Max(float a, float b) { return a > b ? a : b; }
    inline float Min(float a, float b) { return a < b ? a : b; }
    inline float Clamp(float v, float a, float b) { return Min(Max(v, a), b); }
  }
}