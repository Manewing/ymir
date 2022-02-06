#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ymir/Algorithm/Dijkstra.hpp>
#include <ymir/MapIo.hpp>
#include <ymir/Terminal.hpp>

char getHeatMapChar(int Value) {
  if (Value == 0) {
    return '@';
  }
  if (Value < 10) {
    return '0' + Value;
  }
  if (Value - 10 < 26) {
    return 'A' + Value - 10;
  }
  if (Value - 36 < 26) {
    return 'a' + Value - 36;
  }
  return '.';
}

ymir::ColoredUniChar getHeatMapColorChar(int Max, int Value) {
  return {getHeatMapChar(Value),
          ymir::RgbColor::getHeatMapColor(0, Max, Value)};
}

template <typename T, typename U>
ymir::Map<ymir::ColoredUniChar, int> makeHeatMap(const ymir::Map<T, U> &Map,
                                                 const ymir::Map<int, U> &DM) {
  ymir::Map<ymir::ColoredUniChar, U> HM(Map.getSize());
  int MaxDist = *std::max_element(DM.begin(), DM.end());

  Map.forEach([&HM, &DM, MaxDist](auto Pos, auto Tile) {
    int Dist = DM.getTile(Pos);
    if (Dist > 0) {
      auto Char = getHeatMapColorChar(MaxDist, DM.getTile(Pos));
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

int main(int Argc, char *Argv[]) {
  std::filesystem::path MapFile;

  if (Argc == 2) {
    MapFile = Argv[1];
  } else {
    std::cerr << "usage: " << Argv[0] << " <mapfile>" << std::endl;
    return 1;
  }

  auto Map = ymir::loadMap(MapFile);
  auto Starts = Map.findTiles('@');
  auto Ends = Map.findTiles('<');
  if (Starts.empty()) {
    std::cerr << "No start marker '@' found in map " << MapFile << std::endl;
    return 2;
  }
  std::cout << "Map: Starts @ { ";
  const char *Sep = "";
  for (auto const &Start : Starts) {
    std::cout << Sep << Start;
    Sep = ", ";
  }
  std::cout << " }\n" << Map << std::endl;

  auto DM = ymir::Algorithm::getDijkstraMap(
      Map.getSize(), Starts,
      [&Map](ymir::Point2d<int> Pos) { return Map.isTile(Pos, '#'); });
  auto HM = makeHeatMap(Map, DM);
  std::cout << "Dijkstra Map:\n" << HM << std::endl;

  if (Starts.size() == 1) {
    for (const auto &End : Ends) {
      auto Path = ymir::Algorithm::getPathFromDijkstraMap(DM, End);
      std::cout << "Dijkstra Path:\n"
                << markPath(Map, Starts.at(0), End, Path) << std::endl;
    }
  }

  return 0;
}