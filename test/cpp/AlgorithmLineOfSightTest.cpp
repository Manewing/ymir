#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Algorithm/LineOfSight.hpp>

using namespace ymir;

namespace {

void markLineOfSight(ymir::Map<char, int> &Map, ymir::Point2d<int> Pos,
                     unsigned int Range) {
  Algorithm::traverseLOS(
      [&Map](auto SeePos) -> bool {
        if (!Map.contains(SeePos)) {
          return false;
        }
        auto &Tile = Map.getTile(SeePos);
        switch (Tile) {
        case '%':
        case '#':
          Tile = '#';
          return false;
        case ' ':
        case '.':
          Tile = '.';
          break;
        default:
          break;
        }
        return true;
      },
      Pos, Range);
}

struct IsLOSBlocked {
  ymir::Map<char, int> &Map;
  bool operator()(ymir::Point2d<int> Pos) const {
    return Map.getTile(Pos) == '%' || Map.getTile(Pos) == '#';
  }
};

TEST(AlgorithmLineOfSight, TraverseLOSSimpleBox) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%    @     %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPos(Map, '@', ' ');
  markLineOfSight(Map, *Pos, 100);

  Map.getTile(*Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%############%",
      "%#..........#%",
      "%#....@.....#%",
      "%#..........#%",
      "%############%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSSimpleBoxObstacle) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  @ %     %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPos(Map, '@', ' ');
  markLineOfSight(Map, *Pos, 100);

  Map.getTile(*Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%##########%%%",
      "%#........  %%",
      "%#..@.#     %%",
      "%#........  %%",
      "%##########%%%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSNextToWall) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%    @     %%",
      "%%          %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPos(Map, '@', ' ');
  markLineOfSight(Map, *Pos, 100);

  Map.getTile(*Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%%##########%%",
      "%#....@.....#%",
      "%#..........#%",
      "%#..........#%",
      "%############%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSBoxWithBarriers) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %            %%",
      "%%    @                %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPos(Map, '@', ' ');
  markLineOfSight(Map, *Pos, 10);

  Map.getTile(*Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%##########%%%%%%%%%%%%%%",
      "%#........#.....       %%",
      "%#....@..........      %%",
      "%#.............#       %%",
      "%##############%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}


TEST(AlgorithmLineOfSight, TraverseLOSBoxWithBarriersNextToBarrier) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %@           %%",
      "%%                     %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPos(Map, '@', ' ');
  markLineOfSight(Map, *Pos, 10);

  Map.getTile(*Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%#######%#%%%%%%",
      "%%        #@.......... %%",
      "%%       ............  %%",
      "%%     ........#.....  %%",
      "%%%%%%%#########%%%##%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSBoxWithBarriersDistant) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %            %%",
      "%%@                    %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPos(Map, '@', ' ');
  markLineOfSight(Map, *Pos, 10);

  Map.getTile(*Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%##########%%%%%%%%%%%%%%",
      "%#........#            %%",
      "%#@..........          %%",
      "%#..........   %       %%",
      "%##########%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, IsInLOSSimpleBox) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%    @     %%",
      "%%     X    %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPos(Map, '@');
  auto Target = getMarkerPos(Map, 'X');
  ASSERT_TRUE(Start && Target);
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Start, *Target, 10))
      << Map;
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Target, *Start, 10))
      << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSSimpleBoxObstacle) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  @ %     %%",
      "%%     X    %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPos(Map, '@');
  auto Target = getMarkerPos(Map, 'X');
  ASSERT_TRUE(Start && Target);
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Start, *Target, 10))
      << Map;
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Target, *Start, 10))
      << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSSimpleBoxObstacleOutOfLOS) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  @ % X   %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPos(Map, '@');
  auto Target = getMarkerPos(Map, 'X');
  ASSERT_TRUE(Start && Target);
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Start, *Target, 10))
      << Map;
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Target, *Start, 10))
      << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSBoxWithBarries) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %X           %%",
      "%%    @                %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPos(Map, '@');
  auto Target = getMarkerPos(Map, 'X');
  ASSERT_TRUE(Start && Target);
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Start, *Target, 10))
      << Map;
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Target, *Start, 10))
      << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSBoxWithBarriesOutOfLOSDistant) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %X           %%",
      "%%@                    %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPos(Map, '@');
  auto Target = getMarkerPos(Map, 'X');
  ASSERT_TRUE(Start && Target);
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Start, *Target, 10))
      << Map;
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, *Target, *Start, 10))
      << Map;
}

} // namespace