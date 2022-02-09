#include <ymir/Algorithm/DijkstraIo.hpp>

namespace ymir::Algorithm {

ymir::Map<ymir::ColoredUniChar, int>
markPath(const ymir::Map<char, int> &Map, ymir::Point2d<int> Start,
         ymir::Point2d<int> End, const std::vector<ymir::Point2d<int>> &Path) {
  ymir::Map<ymir::ColoredUniChar, int> PM(Map.getSize());
  Map.forEach([&PM](auto Pos, auto Tile) {
    PM.setTile(Pos, {Tile, ymir::NoColor{}});
  });

  auto Red = ymir::RgbColor::getHeatMapColor(0, 20, 0);
  PM.setTiles(Path, {'*', Red});
  PM.setTile(Start, '@');
  PM.setTile(End, '<');
  return PM;
}

} // namespace ymir::Algorithm