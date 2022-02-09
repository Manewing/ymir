#ifndef YMIR_ALGORITHM_DIJKSTRA_IO_HPP
#define YMIR_ALGORITHM_DIJKSTRA_IO_HPP

#include <ymir/Algorithm/Dijkstra.hpp>
#include <ymir/Terminal.hpp>

namespace ymir::Algorithm {

template <typename T, typename U>
ymir::Map<ColoredUniChar, int> makeHeatMap(const ymir::Map<T, U> &Map,
                                           const ymir::Map<int, U> &DM) {
  ymir::Map<ColoredUniChar, U> HM(Map.getSize());
  int MaxDist = *std::max_element(DM.begin(), DM.end());

  Map.forEach([&HM, &DM, MaxDist](auto Pos, auto Tile) {
    int Dist = DM.getTile(Pos);
    if (Dist > 0) {
      auto Char = ColoredUniChar::getHeatMapColorChar(MaxDist, DM.getTile(Pos));
      HM.setTile(Pos, Char);
    } else {
      ymir::ColoredUniChar Char(Tile);
      Char.Color = ymir::NoColor{};
      HM.setTile(Pos, Char);
    }
  });
  return HM;
}

ymir::Map<ymir::ColoredUniChar, int>
markPath(const ymir::Map<char, int> &Map, ymir::Point2d<int> Start,
         ymir::Point2d<int> End, const std::vector<ymir::Point2d<int>> &Path);

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_DIJKSTRA_HPP