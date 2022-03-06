#include "PerlinNoise.hpp"
#include "SimplexNoise.hpp"
#include <ymir/Noise.hpp>

namespace ymir {

namespace Simplex {

float noise2d(float X, float Y, unsigned Octaves, float Persistence,
              float Lacunarity) {
  float Freq = 1.0f;
  float Amp = 1.0f;
  float Max = 1.0f;
  float Total = internal::noise2d(X, Y);

  for (unsigned Idx = 1; Idx < Octaves; ++Idx) {
    Freq *= Lacunarity;
    Amp *= Persistence;
    Max += Amp;
    Total += internal::noise2d(X * Freq, Y * Freq) * Amp;
  }
  return Total / Max;
}

float noise3d(float X, float Y, float Z, unsigned Octaves, float Persistence,
              float Lacunarity) {
  float Freq = 1.0f;
  float Amp = 1.0f;
  float Max = 1.0f;
  float Total = internal::noise3d(X, Y, Z);

  for (unsigned Idx = 1; Idx < Octaves; ++Idx) {
    Freq *= Lacunarity;
    Amp *= Persistence;
    Max += Amp;
    Total += internal::noise3d(X * Freq, Y * Freq, Z * Freq) * Amp;
  }
  return Total / Max;
}

float noise4d(float X, float Y, float Z, float W, unsigned Octaves,
              float Persistence, float Lacunarity) {
  float Freq = 1.0f;
  float Amp = 1.0f;
  float Max = 1.0f;
  float Total = internal::noise4d(X, Y, Z, W);

  for (unsigned Idx = 1; Idx < Octaves; ++Idx) {
    Freq *= Lacunarity;
    Amp *= Persistence;
    Max += Amp;
    Total += internal::noise4d(X * Freq, Y * Freq, Z * Freq, W * Freq) * Amp;
  }
  return Total / Max;
}

} // namespace Simplex

namespace Perlin {

float noise1d(float X, int RepeatX, int Base, unsigned Octaves,
              float Persistence, float Lacunarity) {
  float Freq = 1.0f;
  float Amp = 1.0f;
  float Max = 1.0f;
  float Total = internal::noise1d(X, RepeatX, Base);

  for (unsigned Idx = 1; Idx < Octaves; ++Idx) {
    Freq *= Lacunarity;
    Amp *= Persistence;
    Max += Amp;
    Total += internal::noise1d(X * Freq, RepeatX, Base) * Amp;
  }
  return Total / Max;
}

float noise2d(float X, float Y, int RepeatX, int RepeatY, int Base,
              unsigned Octaves, float Persistence, float Lacunarity) {
  float Freq = 1.0f;
  float Amp = 1.0f;
  float Max = 1.0f;
  float Total = internal::noise2d(X, Y, RepeatX, RepeatY, Base);

  for (unsigned Idx = 1; Idx < Octaves; ++Idx) {
    Freq *= Lacunarity;
    Amp *= Persistence;
    Max += Amp;
    Total +=
        internal::noise2d(X * Freq, Y * Freq, RepeatX, RepeatY, Base) * Amp;
  }
  return Total / Max;
}

float noise3d(float X, float Y, float Z, int RepeatX, int RepeatY, int RepeatZ,
              int Base, unsigned Octaves, float Persistence, float Lacunarity) {
  float Freq = 1.0f;
  float Amp = 1.0f;
  float Max = 1.0f;
  float Total = internal::noise3d(X, Y, Z, RepeatX, RepeatY, RepeatZ, Base);

  for (unsigned Idx = 1; Idx < Octaves; ++Idx) {
    Freq *= Lacunarity;
    Amp *= Persistence;
    Max += Amp;
    Total += internal::noise3d(X * Freq, Y * Freq, Z * Freq, RepeatX, RepeatY,
                               RepeatZ, Base) *
             Amp;
  }
  return Total / Max;
}

} // namespace Perlin

} // namespace ymir