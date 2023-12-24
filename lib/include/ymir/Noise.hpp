#ifndef YMIR_NOISE_HPP
#define YMIR_NOISE_HPP

#include <iterator>
#include <random>
#include <ymir/Map.hpp>
#include <ymir/Types.hpp>

namespace ymir {

// From https://github.com/lemire/testingRNG/blob/master/source/wyhash.h
class WyHashRndEng {
public:
  using result_type = Uint64;

  inline Uint64 operator()() {
    WyHash += 0x60bee2bee120fc15;
    Uint128 Tmp;
    Tmp = (Uint128)WyHash * 0xa3b195354a39b70d;
    Uint64 M1 = Uint64((Tmp >> 64) ^ Tmp);
    Tmp = (Uint128)M1 * 0x1b03738712fad5c9;
    Uint64 M2 = Uint64((Tmp >> 64) ^ Tmp);
    return M2;
  }

  static constexpr inline  Uint64 min() { return 0; }

  static constexpr inline Uint64 max() { return static_cast<Uint64>(-1); }

  inline void seed(Uint64 Seed) { WyHash = Seed; }

private:
  Uint64 WyHash = 0;
};

// REMOVE this just use std::uniform_real_distribution
template <typename RndEngType, typename ValueType = float> class UniformRndEng {
public:
  UniformRndEng(ValueType Begin, ValueType End) : Uniform(Begin, End) {}

  ValueType operator()() { return Uniform(RndEng); }
  void seed(uint64_t Seed) { RndEng.seed(Seed); }

private:
  RndEngType RndEng;
  std::uniform_real_distribution<ValueType> Uniform;
};

template <typename T, typename RE>
Size2d<T> randomSize2d(Rect2d<T> Rect, RE &RndEng) {
  std::uniform_real_distribution<float> Uni(0, 1);
  return {Rect.Pos.X + static_cast<T>(Uni(RndEng) * Rect.Size.W),
          Rect.Pos.Y + static_cast<T>(Uni(RndEng) * Rect.Size.H)};
}

template <typename T, typename RE>
Point2d<T> randomPoint2d(Rect2d<T> Rect, RE &RndEng) {
  std::uniform_real_distribution<float> Uni(0, 1);
  return {Rect.Pos.X + static_cast<T>(Uni(RndEng) * Rect.Size.W),
          Rect.Pos.Y + static_cast<T>(Uni(RndEng) * Rect.Size.H)};
}

template <typename TileType, typename RndGenType, typename U>
void fillRectRandom(Map<TileType, U> &M, TileType Tile, float Chance,
                    RndGenType &RndGen,
                    std::optional<Rect2d<typename nd<U>::type>> Rect = {}) {
  std::uniform_int_distribution<int> Uni(0, 1000000);
  int IntChance = Chance * 1000000;
  M.forEachElem(
      [&Uni, &RndGen, IntChance, Tile](TileType &MapTile) {
        if (Uni(RndGen) < IntChance) {
          MapTile = Tile;
        }
      },
      Rect);
}

template <typename TileType, typename RndGenType, typename U>
void fillRectSeedRandom(Map<TileType, U> &M, TileType Tile, float Chance,
                        RndGenType &RndGen,
                        std::optional<Rect2d<typename nd<U>::type>> Rect = {},
                        Point2d<U> Offset = {0, 0}) {
  std::uniform_int_distribution<int> Uni(0, 1000000);
  int IntChance = Chance * 1000000;
  M.forEach(
      [&Uni, &RndGen, IntChance, Tile, Offset](Point2d<U> P,
                                               TileType &MapTile) {
        RndGen.seed((P.X + Offset.X) | (P.Y + Offset.Y));
        if (Uni(RndGen) < IntChance) {
          MapTile = Tile;
        }
      },
      Rect);
}

template <typename Iter, typename RE>
Iter randomIterator(Iter Begin, Iter End, RE &RndEng) {
  std::uniform_int_distribution<int> Uni(0, std::distance(Begin, End) - 1);
  std::advance(Begin, Uni(RndEng));
  return Begin;
}

namespace Simplex {

float noise2d(float X, float Y, unsigned Octaves, float Persistence,
              float Lacunarity);

float noise3d(float X, float Y, float Z, unsigned Octaves, float Persistence,
              float Lacunarity);

float noise4d(float X, float Y, float Z, float W, unsigned Octaves,
              float Persistence, float Lacunarity);

template <typename TileCord>
Map<float, TileCord>
generateNoiseMap(Size2d<TileCord> Size, Point2d<TileCord> Offset = {0, 0},
                 int Seed = 0, float Scale = 1.0f, int Octaves = 1,
                 float Persistence = 0.5f, float Lacunarity = 2.0f) {
  Map<float, TileCord> M(Size);
  M.forEach([Seed, Scale, Offset, Octaves, Persistence, Lacunarity](auto Pos,
                                                                    auto &T) {
    auto GblPos = Pos + Offset;
    T = noise3d(float(GblPos.X) / Scale, float(GblPos.Y) / Scale, Seed, Octaves,
                Persistence, Lacunarity);
  });
  return M;
}

} // namespace Simplex

namespace Perlin {

float noise1d(float X, int RepeatX, int Base, unsigned Octaves,
              float Persistence, float Lacunarity);

float noise2d(float X, float Y, int RepeatX, int RepeatY, int Base,
              unsigned Octaves, float Persistence, float Lacunarity);

float noise3d(float X, float Y, float Z, int RepeatX, int RepeatY, int RepeatZ,
              int Base, unsigned Octaves, float Persistence, float Lacunarity);

} // namespace Perlin

} // namespace ymir

#endif // #ifndef YMIR_NOISE_HPP