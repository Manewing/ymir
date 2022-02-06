#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Dungeon/Door.hpp>

using namespace ymir;
using namespace ymir::Dungeon;

namespace {

TEST(DungeonDoorTest, Attributes) {
  Door<int> D;
  EXPECT_EQ(D.Pos, Point2d<int>(0, 0));
  EXPECT_EQ(D.Dir, Dir2d::NONE);
  EXPECT_EQ(D.Used, false);
  EXPECT_EQ(D, D);
  auto DPos = D;
  D.Pos = {1, 2};
  EXPECT_NE(D, DPos);
}

TEST(DungeonDoorTest, Output) {
  Door<int> D = {{1, 5}, Dir2d::RIGHT, true};
  EXPECT_EQ(dump(D), "Door{Point2d{1, 5}, Dir2d(RIGHT), /*Used=*/true}");
}

} // namespace