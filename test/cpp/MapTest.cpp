#include <gtest/gtest.h>
#include <ymir/Map.hpp>
#include <ymir/MapIo.hpp>

namespace {

TEST(MapTest, MinMax) {
  auto Map = ymir::loadMap({
      "# #",
      "#x#",
      "###",
  });
  auto MinElem = std::min_element(Map.begin(), Map.end());
  EXPECT_EQ(*MinElem, ' ');
  EXPECT_EQ(Map.toPos(MinElem), ymir::Point2d<int>(1, 0));
  auto MaxElem = std::max_element(Map.begin(), Map.end());
  EXPECT_EQ(*MaxElem, 'x');
  EXPECT_EQ(Map.toPos(MaxElem), ymir::Point2d<int>(1, 1));
}

}