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

TEST(AlgorithmLineOfSight, SimpleBox) {
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
      "%%##########%%",
      "%#..........#%",
      "%#....@.....#%",
      "%#..........#%",
      "%%##########%%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, NextToWall) {
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
      "%%%#######%%%%",
      "%#....@.....#%",
      "%#..........#%",
      "%#..........#%",
      "%%##########%%",
      "%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmLineOfSight, BoxWithBarriers) {
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
      "%%########%%%%%%%%%%%%%%%",
      "%#........# .....      %%",
      "%#....@..........      %%",
      "%#.............#       %%",
      "%%##########%#%%%%%%%%%%%",
      "%%%%%%%%%%%%%%%%%%%%%%%%%",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

} // namespace