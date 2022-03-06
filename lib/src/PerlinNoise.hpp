/* From https://github.com/caseman/noise
 *
 * Copyright (c) 2008 Casey Duncan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef YMIR_PERLIN_NOISE_HPP
#define YMIR_PERLIN_NOISE_HPP

#include "SimplexPerlinNoise.hpp"

namespace ymir::Perlin::internal {

#define lerp(t, a, b) ((a) + (t) * ((b) - (a)))

inline float grad1(const int hash, const float x) {
  float g = (hash & 7) + 1.0f;
  if (hash & 8)
    g = -1;
  return (g * x);
}

inline float noise1d(float x, const int repeat, const int base) {
  float fx;
  int i = (int)floorf(x) % repeat;
  int ii = (i + 1) % repeat;
  i = (i & 255) + base;
  ii = (ii & 255) + base;

  x -= floorf(x);
  fx = x * x * x * (x * (x * 6 - 15) + 10);

  return lerp(fx, grad1(PERM[i], x), grad1(PERM[ii], x - 1)) * 0.4f;
}

inline float grad2(const int hash, const float x, const float y) {
  const int h = hash & 15;
  return x * GRAD3[h][0] + y * GRAD3[h][1];
}

inline float noise2d(float x, float y, const float repeatx, const float repeaty,
                     const int base) {
  float fx, fy;
  int A, AA, AB, B, BA, BB;
  int i = (int)floorf(fmodf(x, repeatx));
  int j = (int)floorf(fmodf(y, repeaty));
  int ii = (int)fmodf(i + 1, repeatx);
  int jj = (int)fmodf(j + 1, repeaty);
  i = (i & 255) + base;
  j = (j & 255) + base;
  ii = (ii & 255) + base;
  jj = (jj & 255) + base;

  x -= floorf(x);
  y -= floorf(y);
  fx = x * x * x * (x * (x * 6 - 15) + 10);
  fy = y * y * y * (y * (y * 6 - 15) + 10);

  A = PERM[i];
  AA = PERM[A + j];
  AB = PERM[A + jj];
  B = PERM[ii];
  BA = PERM[B + j];
  BB = PERM[B + jj];

  return lerp(
      fy, lerp(fx, grad2(PERM[AA], x, y), grad2(PERM[BA], x - 1, y)),
      lerp(fx, grad2(PERM[AB], x, y - 1), grad2(PERM[BB], x - 1, y - 1)));
}

inline float grad3(const int hash, const float x, const float y,
                   const float z) {
  const int h = hash & 15;
  return x * GRAD3[h][0] + y * GRAD3[h][1] + z * GRAD3[h][2];
}

inline float noise3d(float x, float y, float z, const int repeatx,
                     const int repeaty, const int repeatz, const int base) {
  float fx, fy, fz;
  int A, AA, AB, B, BA, BB;
  int i = (int)floorf(fmodf(x, repeatx));
  int j = (int)floorf(fmodf(y, repeaty));
  int k = (int)floorf(fmodf(z, repeatz));
  int ii = (int)fmodf(i + 1, repeatx);
  int jj = (int)fmodf(j + 1, repeaty);
  int kk = (int)fmodf(k + 1, repeatz);
  i = (i & 255) + base;
  j = (j & 255) + base;
  k = (k & 255) + base;
  ii = (ii & 255) + base;
  jj = (jj & 255) + base;
  kk = (kk & 255) + base;

  x -= floorf(x);
  y -= floorf(y);
  z -= floorf(z);
  fx = x * x * x * (x * (x * 6 - 15) + 10);
  fy = y * y * y * (y * (y * 6 - 15) + 10);
  fz = z * z * z * (z * (z * 6 - 15) + 10);

  A = PERM[i];
  AA = PERM[A + j];
  AB = PERM[A + jj];
  B = PERM[ii];
  BA = PERM[B + j];
  BB = PERM[B + jj];

  return lerp(fz,
              lerp(fy,
                   lerp(fx, grad3(PERM[AA + k], x, y, z),
                        grad3(PERM[BA + k], x - 1, y, z)),
                   lerp(fx, grad3(PERM[AB + k], x, y - 1, z),
                        grad3(PERM[BB + k], x - 1, y - 1, z))),
              lerp(fy,
                   lerp(fx, grad3(PERM[AA + kk], x, y, z - 1),
                        grad3(PERM[BA + kk], x - 1, y, z - 1)),
                   lerp(fx, grad3(PERM[AB + kk], x, y - 1, z - 1),
                        grad3(PERM[BB + kk], x - 1, y - 1, z - 1))));
}

} // namespace ymir::Perlin::internal

#endif // #ifndef YMIR_PERLIN_NOISE_HPP