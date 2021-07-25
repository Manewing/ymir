#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Dungeon/Room.hpp>

using namespace ymir;
using namespace ymir::Dungeon;

namespace {

TEST(DungeonRoomTest, Attributes) {
  Map<char, int> M(10, 10);
  Room<char, int> R{M, {}};
  EXPECT_TRUE(R.Doors.empty());
  EXPECT_EQ(R.Pos, Point2d<int>(0, 0));
  EXPECT_EQ(R.rect(), Rect2d<int>({0, 0}, {10, 10}));
}

TEST(DungeonRoomTest, GetDoorCandidates) {
  {
    Map<char, int> M = getMap({"################", //
                               "################", //
                               "####      ######", //
                               "####        ####", //
                               "####        ####", //
                               "####      ######", //
                               "#####   ########", //
                               "################"});
    auto Doors = getDoorCandidates(M, ' ');
    Room<char, int> R{M, Doors};

    Map<char, int> MRef = getMap({"################", //
                                  "#####^^^^#######", //
                                  "####      ######", //
                                  "###<        ####", //
                                  "###<        ####", //
                                  "####      ######", //
                                  "#####   ########", //
                                  "######v#########"});
    internal::markDoors(R.M, R.Doors);
    EXPECT_EQ(R.M, MRef);
  }
  {
    Map<char, int> M = getMap({"################", //
                               "################", //
                               "################", //
                               "####   #########", //
                               "####   #########", //
                               "####   #########", //
                               "################", //
                               "################"});
    auto Doors = getDoorCandidates(M, ' ');
    Room<char, int> R{M, Doors};

    Map<char, int> MRef = getMap({"################", //
                                  "################", //
                                  "#####^##########", //
                                  "####   #########", //
                                  "###<   >########", //
                                  "####   #########", //
                                  "#####v##########", //
                                  "################"});
    internal::markDoors(R.M, R.Doors);
    EXPECT_EQ(R.M, MRef);
  }
}

TEST(DungeonRoomTest, GetDoor) {
  using Door = ymir::Dungeon::Door<int>;
  // Empty
  {
    Room<char, int> R{Map<char, int>({}), {}};
    EXPECT_EQ(R.getDoor(Dir2d::DOWN), std::nullopt);
  }
  // Non-empty
  {
    Door Up{{0, 0}, Dir2d::UP, false};
    Door Down{{0, 0}, Dir2d::DOWN, false};
    Door Left{{0, 0}, Dir2d::LEFT, false};
    Door Right{{0, 0}, Dir2d::RIGHT, false};
    Room<char, int> R{Map<char, int>({}), {Up, Down, Left, Right}};
    EXPECT_EQ(R.getDoor(Dir2d::DOWN), Down);
    EXPECT_EQ(R.getDoor(Dir2d::UP), Up);
    EXPECT_EQ(R.getDoor(Dir2d::RIGHT), Right);
    EXPECT_EQ(R.getDoor(Dir2d::HORIZONTAL), Left);
  }
}

TEST(DungeonRoomTest, GetDoorDirections) {
  using Door = ymir::Dungeon::Door<int>;

  // Empty
  {
    Room<char, int> R{Map<char, int>({}), {}};
    EXPECT_EQ(R.getDoorDirections(), Dir2d::NONE);
  }
  // Non-empty
  {
    Door Left{{0, 0}, Dir2d::LEFT, false};
    Door Right{{0, 0}, Dir2d::RIGHT, false};
    Room<char, int> R{Map<char, int>({}), {Left, Right}};
    EXPECT_EQ(R.getDoorDirections(), Dir2d::HORIZONTAL);
  }
}

TEST(DungeonRoomTest, GetOpposingDoorDirections) {
  using Door = ymir::Dungeon::Door<int>;

  // Empty
  {
    Room<char, int> R{Map<char, int>({}), {}};
    EXPECT_EQ(R.getOpposingDoorDirections(), Dir2d::NONE);
  }
  // Non-empty
  {
    Door Up{{0, 0}, Dir2d::UP, false};
    Door Right{{0, 0}, Dir2d::RIGHT, false};
    Room<char, int> R{Map<char, int>({}), {Up, Right}};
    EXPECT_EQ(R.getOpposingDoorDirections(), Dir2d::DOWN | Dir2d::LEFT);
  }
}

} // namespace
