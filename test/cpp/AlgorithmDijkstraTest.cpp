#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Algorithm/Dijkstra.hpp>

using namespace ymir;

namespace {

using FourTileDirections = ymir::Algorithm::FourTileDirections<int>;
using EightTileDirections = ymir::Algorithm::EightTileDirections<int>;

template <typename DirectionProvider>
void findAndMarkPath(ymir::Map<char, int> &Map) {
  auto Start = Map.findTiles('S').at(0);
  auto End = Map.findTiles('E').at(0);
  Map.replaceTile('S', ' ');
  Map.replaceTile('E', ' ');
  auto DM = ymir::Algorithm::getDijkstraMap(
      Map.getSize(), Start,
      [&Map](auto Pos) { return Map.getTile(Pos) != ' '; },
      DirectionProvider());
  auto Path =
      ymir::Algorithm::getPathFromDijkstraMap(DM, End, DirectionProvider());
  EXPECT_FALSE(Path.empty());
  Map.setTiles(Path, 'x');
}

TEST(AlgorithmDijkstraTest, FindPathFourTile) {
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