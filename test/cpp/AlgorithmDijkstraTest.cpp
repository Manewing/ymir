#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Algorithm/Dijkstra.hpp>

using namespace ymir;

namespace {

using FourTileDirections = ymir::FourTileDirections<int>;
using EightTileDirections = ymir::EightTileDirections<int>;

template <typename DirectionProvider>
void findAndMarkPath(ymir::Map<char, int> &Map, unsigned MaxLength = -1) {
  auto Start = Map.findTiles('S').at(0);
  auto End = Map.findTiles('E').at(0);
  Map.replaceTile('S', ' ');
  Map.replaceTile('E', ' ');
  auto DM = ymir::Algorithm::getDijkstraMap(
      Map.getSize(), Start,
      [&Map](auto Pos) { return Map.getTile(Pos) != ' '; },
      DirectionProvider());
  auto Path = ymir::Algorithm::getPathFromDijkstraMap(
      DM, End, DirectionProvider(), MaxLength);
  Map.setTiles(Path, 'x');
}

TEST(AlgorithmDijkstraTest, FindPathFourTileTunnel) {
  auto Map = loadMap({
      "##############",
      "##############",
      "#S   #########",
      "#           ##",
      "########### ##",
      "##E         ##",
      "##############",
  });
  findAndMarkPath<FourTileDirections>(Map);

  auto MapRef = loadMap({
      "##############",
      "##############",
      "#x   #########",
      "#xxxxxxxxxxx##",
      "###########x##",
      "##xxxxxxxxxx##",
      "##############",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmDijkstraTest, FindPathMaxLength) {
  auto Map = loadMap({
      "##############",
      "##############",
      "#E   #########",
      "#           ##",
      "########### ##",
      "##S         ##",
      "##############",
  });
  findAndMarkPath<FourTileDirections>(Map, 6);

  auto MapRef = loadMap({
      "##############",
      "##############",
      "#xxxx#########",
      "#   xxx     ##",
      "########### ##",
      "##          ##",
      "##############",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmDijkstraTest, FindPathFourTileTunnelInverted) {
  auto Map = loadMap({
      "##############",
      "##############",
      "#E   #########",
      "#           ##",
      "########### ##",
      "##S         ##",
      "##############",
  });
  findAndMarkPath<FourTileDirections>(Map);

  auto MapRef = loadMap({
      "##############",
      "##############",
      "#xxxx#########",
      "#   xxxxxxxx##",
      "###########x##",
      "##xxxxxxxxxx##",
      "##############",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmDijkstraTest, FindPathFourTileBlocked) {
  auto Map = loadMap({
      "##############",
      "#####  S  ####",
      "#   ##########",
      "#           ##",
      "#####  E    ##",
      "##          ##",
      "##############",
  });
  findAndMarkPath<FourTileDirections>(Map);

  auto MapRef = loadMap({
      "##############",
      "#####     ####",
      "#   ##########",
      "#           ##",
      "#####       ##",
      "##          ##",
      "##############",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmDijkstraTest, FindPathFourTileOpenCave) {
  auto Map = loadMap({
      "##############",
      "#####  S  ####",
      "#  ##   ######",
      "#      #### ##",
      "#####  E    ##",
      "##       #####",
      "##############",
  });
  findAndMarkPath<FourTileDirections>(Map);

  auto MapRef = loadMap({
      "##############",
      "#####  x  ####",
      "#  ## xx######",
      "#     x#### ##",
      "##### xx    ##",
      "##       #####",
      "##############",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

TEST(AlgorithmDijkstraTest, FindPathEightTile) {
  auto Map = loadMap({
      "##############",
      "##############",
      "#S   #########",
      "#           ##",
      "########### ##",
      "##E         ##",
      "##############",
  });
  findAndMarkPath<EightTileDirections>(Map);

  auto MapRef = loadMap({
      "##############",
      "##############",
      "#x   #########",
      "# xxxxxxxxx ##",
      "###########x##",
      "##xxxxxxxxx ##",
      "##############",
  });
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef;
}

} // namespace