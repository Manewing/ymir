#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Dungeon/Room.hpp>
#include <ymir/MapIo.hpp>

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
    Map<char, int> M = loadMap({"################", //
                                "################", //
                                "####      ######", //
                                "####        ####", //
                                "####        ####", //
                                "####      ######", //
                                "#####   ########", //
                                "################"});
    auto Doors = getDoorCandidates(M, ' ');
    Room<char, int> R{M, Doors};

    Map<char, int> MRef = loadMap({"################", //
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
    Map<char, int> M = loadMap({"################", //
                                "################", //
                                "################", //
                                "####   #########", //
                                "####   #########", //
                                "####   #########", //
                                "################", //
                                "################"});
    auto Doors = getDoorCandidates(M, ' ');
    Room<char, int> R{M, Doors};

    Map<char, int> MRef = loadMap({"################", //
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
    EXPECT_EQ(R.getDoor(Dir2d::DOWN), nullptr);
  }
  // Non-empty
  {
    Door Up{{0, 0}, Dir2d::UP, false};
    Door Down{{0, 0}, Dir2d::DOWN, false};
    Door Left{{0, 0}, Dir2d::LEFT, false};
    Door Right{{0, 0}, Dir2d::RIGHT, false};
    Room<char, int> R{Map<char, int>({}), {Up, Down, Left, Right}};
    EXPECT_EQ(R.getDoor(Dir2d::NONE), nullptr);
    EXPECT_EQ(*R.getDoor(Dir2d::DOWN), Down);
    EXPECT_EQ(*R.getDoor(Dir2d::UP), Up);
    EXPECT_EQ(*R.getDoor(Dir2d::RIGHT), Right);
    EXPECT_EQ(*R.getDoor(Dir2d::HORIZONTAL), Left);
  }
}

TEST(DungeonRoomTest, GetNumUsedDoors) {
  // Empty
  {
    Room<char, int> R{Map<char, int>({}), {}};
    EXPECT_EQ(R.getNumUsedDoors(), 0);
  }
  // Non-Empty
  {
    Room<char, int> R{Map<char, int>({}),
                      {
                          {{0, 0}, Dir2d::LEFT, true},
                          {{0, 1}, Dir2d::LEFT, false},
                          {{0, 2}, Dir2d::LEFT, false},
                          {{13, 6}, Dir2d::RIGHT, false},
                          {{14, 6}, Dir2d::RIGHT, true},
                          {{15, 6}, Dir2d::RIGHT, false},
                      }};
    EXPECT_EQ(R.getNumUsedDoors(), 2);
  }
}

TEST(DungeonRoomTest, BlocksDoor) {
  // Empty
  {
    Room<char, int> R{Map<char, int>({}), {}};
    EXPECT_FALSE(R.blocksDoor({4, 3}, /*Used=*/false));
  }

  // Non-empty
  {
    Room<char, int> R{Map<char, int>({}),
                      {{{0, 0}, Dir2d::LEFT, true},
                       {{0, 1}, Dir2d::LEFT, false},
                       {{0, 2}, Dir2d::LEFT, false},
                       {{13, 6}, Dir2d::RIGHT, false},
                       {{13, 7}, Dir2d::RIGHT, true},
                       {{13, 8}, Dir2d::RIGHT, false},
                       {{20, 20}, Dir2d::DOWN, false},
                       {{10, 4}, Dir2d::UP, false}}};
    EXPECT_TRUE(R.blocksDoor({0, 0}, /*Used=*/true));
    EXPECT_TRUE(R.blocksDoor({1, 0}, /*Used=*/true));
    EXPECT_TRUE(R.blocksDoor({12, 7}, /*Used=*/true));
    EXPECT_TRUE(R.blocksDoor({13, 7}, /*Used=*/true));
    EXPECT_FALSE(R.blocksDoor({1, 1}, /*Used=*/true));
    EXPECT_FALSE(R.blocksDoor({15, 17}, /*Used=*/true));
    EXPECT_TRUE(R.blocksDoor({20, 20}, /*Used=*/false));
    EXPECT_TRUE(R.blocksDoor({20, 19}, /*Used=*/false));
    EXPECT_TRUE(R.blocksDoor({10, 4}, /*Used=*/false));
    EXPECT_TRUE(R.blocksDoor({10, 5}, /*Used=*/false));
    EXPECT_FALSE(R.blocksDoor({0, 0}, /*Used=*/false));
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
