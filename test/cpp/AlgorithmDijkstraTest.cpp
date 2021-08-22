#include "TestHelpers.hpp"
#include <gtest/gtest.h>
#include <ymir/Algorithm/Dijkstra.hpp>

using namespace ymir;

namespace {

TEST(AlgorithmDijkstraTest, FindPath) {
  auto Map = loadMap({
      "##############",
      "##############",
      "#S   #########",
      "#           ##",
      "########### ##",
      "##E         ##",
      "##############",
  });

  auto Start = Map.findTiles('S').at(0);
  auto End = Map.findTiles('E').at(0);
  Map.replaceTile('S', ' ');
  Map.replaceTile('E', ' ');

  auto DM =
      ymir::Algorithm::getDijkstraMap(Map.getSize(), Start, [&Map](auto Pos) {
        return Map.getTile(Pos) != ' ';
      });
  auto Path = ymir::Algorithm::getPathFromDijkstraMap(DM, End);
  EXPECT_FALSE(Path.empty());
  Map.setTiles(Path, 'x');

  auto MapRef = loadMap({
      "##############",
      "##############",
      "#x   #########",
      "#xxxxxxxxxxx##",
      "###########x##",
      "##xxxxxxxxxx##",
      "##############",
  });
  EXPECT_EQ(Map, MapRef);
}

} // namespace