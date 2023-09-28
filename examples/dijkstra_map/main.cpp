#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ymir/Algorithm/Dijkstra.hpp>
#include <ymir/Algorithm/DijkstraIo.hpp>
#include <ymir/MapIo.hpp>

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
  auto HM = ymir::Algorithm::makeHeatMap(Map, DM);
  std::cout << "Dijkstra Map:\n" << HM << std::endl;

  if (Starts.size() == 1) {
    for (const auto &End : Ends) {
      auto Path = ymir::Algorithm::getPathFromDijkstraMap(DM, Starts.at(0), End);
      std::cout << "Dijkstra Path:\n"
                << ymir::Algorithm::markPath(Map, Starts.at(0), End, Path)
                << std::endl;
    }
  }

  return 0;
}