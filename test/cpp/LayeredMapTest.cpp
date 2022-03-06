#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/LayeredMap.hpp>
#include <ymir/Map.hpp>
#include <ymir/MapIo.hpp>

namespace {

TEST(LayeredMapTest, Render) {
  ymir::LayeredMap<char, int> LM({"walls", "objects"}, {3, 3});
  LM.get("walls") = ymir::loadMap({
      "# #",
      "# #",
      "###",
  });
  LM.get("objects") = ymir::loadMap({
      "   ",
      " x ",
      "   ",
  });
  auto RenderedMap = LM.render(' ');
  auto RefRenderedMap = ymir::loadMap({
      "# #",
      "#x#",
      "###",
  });
  EXPECT_MAP_EQ(RenderedMap, RefRenderedMap);
}

} // namespace