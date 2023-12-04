#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Algorithm/LineOfSight.hpp>

using namespace ymir;

namespace {

struct IsLOSBlocked {
  ymir::Map<char, int> &Map;
  bool operator()(ymir::Point2d<int> Pos) const {
    return Map.getTile(Pos) == '%' || Map.getTile(Pos) == '#';
  }
};

void markLineOfSight(ymir::Map<char, int> &Map, ymir::Point2d<int> Pos,
                     unsigned int Range) {
  Algorithm::shadowCasting<int>(
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
        case '*':
          Tile = '@';
          break;
        default:
          break;
        }
        return true;
      },
      IsLOSBlocked{Map}, Pos, Range);
}

void markRayCastDDA(ymir::Map<char, int> &Map, ymir::Point2d<int> Start,
                    ymir::Point2d<int> Target, unsigned int Range) {
  auto const Offset = Point2d<double>(0.5, 0.5);
  Algorithm::rayCastDDA<int>(
      [&Map](auto SeePos) {
        if (!Map.contains(SeePos)) {
          return;
        }
        auto &Tile = Map.getTile(SeePos);
        if (Tile >= 'A' && Tile <= 'Z') {
          Tile = char(Tile - 'A' + 'a');
          return;
        }
        switch (Tile) {
        case '%':
        case '#':
          Tile = '#';
          break;
        case ' ':
        case '.':
          Tile = '.';
          break;
        case '*':
          Tile = '@';
          break;
        default:
          break;
        }
      },
      IsLOSBlocked{Map}, Range, Start.to<double>() + Offset,
      Target.to<double>() + Offset);
}

TEST(AlgorithmLineOfSight, RayCastDDASimple) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%    *  Y  %%",
      "%%X         %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  auto TargetX = getMarkerPosOrFail(Map, 'X');
  auto TargetY = getMarkerPosOrFail(Map, 'Y');

  markRayCastDDA(Map, Pos, TargetX, 10);
  markRayCastDDA(Map, Pos, TargetY, 10);

  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  ..*..y..#%",
      "%#x..       %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, RayCastDDACorner) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  * %     %%",
      "%%    X     %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  auto TargetX = getMarkerPosOrFail(Map, 'X');

  markRayCastDDA(Map, Pos, TargetX, 10);

  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  *.%     %%",
      "%%   .x.    %%",
      "%%%%%%%#%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, RayCastDDASimpleBarriers) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %X           %%",
      "%%*                    %%",
      "%%       %     %       %%",
      "%%%%%%%%%% Y %%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPosOrFail(Map, '*');
  auto TargetX = getMarkerPosOrFail(Map, 'X');
  auto TargetY = getMarkerPosOrFail(Map, 'Y');

  markRayCastDDA(Map, Start, TargetX, 10);
  markRayCastDDA(Map, Start, TargetY, 10);

  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%     ...#X           %%",
      "%%*.....               %%",
      "%%  .....#     %       %%",
      "%%%%%%%%%% Y %%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSSmallBox) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%    *     %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 10);

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

TEST(AlgorithmLineOfSight, TraverseLOSLargeBox) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                     *                     %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%                                           %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 10);

  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%#########%%%%%%%%%%%%%%%%%%%",
      "%%               .............               %%",
      "%%              ...............              %%",
      "%%             .................             %%",
      "%%             .................             %%",
      "%%            ...................            %%",
      "%%            ...................            %%",
      "%%            ...................            %%",
      "%%            ...................            %%",
      "%%           ..........@..........           %%",
      "%%            ...................            %%",
      "%%            ...................            %%",
      "%%            ...................            %%",
      "%%            ...................            %%",
      "%%             .................             %%",
      "%%             .................             %%",
      "%%              ...............              %%",
      "%%               .............               %%",
      "%%                 .........                 %%",
      "%%%%%%%%%%%%%%%%%%%%%%%#%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSSimpleBoxObstacle) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  * %     %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 100);

  Map.getTile(Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%#########%%%%",
      "%#.......   %%",
      "%#..@.#     %%",
      "%#.......   %%",
      "%#########%%%%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSNextToWall) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%    *     %%",
      "%%          %%",
      "%%          %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 100);

  Map.getTile(Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%",
      "%############%",
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
      "%%    *                %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 10);

  Map.getTile(Pos) = '@';
  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%##########%%%%%%%%%%%%%%",
      "%#........#.....       %%",
      "%#....@..........      %%",
      "%#.............#       %%",
      "%###############%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSBoxWithBarriersNextToBarrier) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %*           %%",
      "%%                     %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 10);

  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%###########%%%%",
      "%%        #@.......... %%",
      "%%        ...........  %%",
      "%%       ......#.....  %%",
      "%%%%%%%%########%%%##%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, TraverseLOSBoxWithBarriersDistant) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %            %%",
      "%%*                    %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Pos = getMarkerPosOrFail(Map, '*');
  markLineOfSight(Map, Pos, 10);

  auto MapRef = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%##########%%%%%%%%%%%%%%",
      "%#........#.           %%",
      "%#@..........          %%",
      "%#..........   %       %%",
      "%###########%%%%%%%%%%%%%",
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
  auto Start = getMarkerPosOrFail(Map, '@');
  auto Target = getMarkerPosOrFail(Map, 'X');
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, Start, Target, 10)) << Map;
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, Target, Start, 10)) << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSSimpleBoxObstacle) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
      "%%          %%",
      "%%  @ %     %%",
      "%%    X     %%",
      "%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPosOrFail(Map, '@');
  auto Target = getMarkerPosOrFail(Map, 'X');
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, Start, Target, 10)) << Map;
  EXPECT_TRUE(Algorithm::isInLOS(IsLOSBlocked{Map}, Target, Start, 10)) << Map;
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
  auto Start = getMarkerPosOrFail(Map, '@');
  auto Target = getMarkerPosOrFail(Map, 'X');
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, Start, Target, 10)) << Map;
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, Target, Start, 10)) << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSBoxWithBarriers) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %X           %%",
      "%%    @   %            %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPosOrFail(Map, '@');
  auto Target = getMarkerPosOrFail(Map, 'X');
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, Start, Target, 10)) << Map;
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, Target, Start, 10)) << Map;
}

TEST(AlgorithmLineOfSight, IsInLOSBoxWithBarriersOutOfLOSDistant) {
  auto Map = loadMap({
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%        %X           %%",
      "%%@                    %%",
      "%%             %       %%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  auto Start = getMarkerPosOrFail(Map, '@');
  auto Target = getMarkerPosOrFail(Map, 'X');
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, Start, Target, 10)) << Map;
  EXPECT_FALSE(Algorithm::isInLOS(IsLOSBlocked{Map}, Target, Start, 10)) << Map;
}

} // namespace