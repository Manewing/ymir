#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Map.hpp>
#include <ymir/Noise.hpp>
#include <ymir/Algorithm/MapAlgebra.hpp>

namespace {

ymir::Map<char, int> toCharMap(const ymir::Map<float, int> &NoiseMap) {
  ymir::Map<char, int> M(NoiseMap.getSize());

  M.forEach([NoiseMap](auto Pos, auto &T) {
    auto Value = NoiseMap.getTile(Pos);
    if (Value < -0.65) {
      T = ' ';
    } else if (Value < -0.5) {
      T = '-';
    } else if (Value < -0.3) {
      T = ':';
    } else if (Value < 0) {
      T = '=';
    } else if (Value < 0.3) {
      T = '*';
    } else if (Value < 0.5) {
      T = 'x';
    } else if (Value < 0.65) {
      T = '0';
    } else {
      T = '#';
    }
  });

  return M;
}

TEST(NoiseTest, SimplexNoiseMap) {
  auto NoiseMap = ymir::Simplex::generateNoiseMap<int>({20, 20}, {0, 0}, 0,
                                                       64.0f, 6, 0.5f, 2.0f);
  auto Map = toCharMap(NoiseMap);
  auto MapRef = ymir::loadMap({
      // clang-format off
      "***xxxxxxxxxxxxxxxxx",
      "**xxxxxxx000xxxxxxxx",
      "*xxx0000000000xxxxxx",
      "*xx000000000000xxxx0",
      "xxx000000000000000xx",
      "xx000000###00000000x",
      "xx00000###000000000x",
      "xx000000000000000000",
      "x0000000000#00000000",
      "x0000000000#00000000",
      "xx000000000000000000",
      "xx000000000000000000",
      "xxx000000000000000xx",
      "xx00000000000000xxxx",
      "x000000x000000xxxxxx",
      "x00000xxxxxxxxxxxxxx",
      "x000xxxxxxxxxxxxx*xx",
      "x0xxxxxxxxxxxx*****x",
      "xxxxxxxxxxxxxx******",
      "xxxxxxxxxxxxx*******",
      // clang-format on
  });
  EXPECT_MAP_EQ(Map, MapRef);
}

TEST(NoiseTest, SimplexNoiseMapOverlayed) {
  auto NoiseMap = ymir::Simplex::generateNoiseMap<int>({20, 20}, {0, 0}, 0,
                                                       64.0f, 6, 0.5f, 2.0f);
  auto NoiseMapHigh = ymir::Simplex::generateNoiseMap<int>({20, 20}, {0, 0}, 0,
                                                       8.0f, 6, 0.5f, 2.0f);
  auto [HM, HMH] = ymir::Algorithm::algebra(NoiseMap, NoiseMapHigh);
  HM = ((HM * 15.0f) + HMH) / 16.0f;

  auto Map = toCharMap(NoiseMap);
  auto MapRef = ymir::loadMap({
      // clang-format off
      "***xxxxxxxxxxxxxxxxx",
      "**xxxxxxxx0xxxxxxxxx",
      "*xxxx000000000xxxxxx",
      "*xxx0000000000xxxxx0",
      "xxxxx0000000000xxxxx",
      "xxxxx000000000000xxx",
      "xxx000000000000000xx",
      "xx000000000000000000",
      "x0000000000000000000",
      "xxxx0000xx0000x00000",
      "xxx00000xx0000000000",
      "xxxx00000x0000000000",
      "xxxxx000xx00000000xx",
      "xxxx000x0000000xxxxx",
      "xxxxx0xxx00x0xxxxxxx",
      "x0xxxxxxxxxxxxxxxxxx",
      "x0xxxxxxxxxxxxx*****",
      "x0xxxxxxxxxxxx******",
      "xxxxxxxxxxxxx*******",
      "xxxxxxxxxxxx********",
      // clang-format on
  });
  EXPECT_MAP_EQ(Map, MapRef);
}



} // namespace