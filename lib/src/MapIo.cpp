#include <algorithm>
#include <fstream>
#include <ymir/MapIo.hpp>

namespace ymir {

ymir::Map<char, int> loadMap(const std::vector<std::string> &MapRepr) {
  if (MapRepr.empty()) {
    return ymir::Map<char, int>(0, 0);
  }
  const auto Height = MapRepr.size();
  const auto Width = MapRepr.at(0).size();
  const auto AllSameWidth = std::all_of(
      MapRepr.begin(), MapRepr.end(),
      [Width](const std::string &Str) { return Str.size() == Width; });
  if (!AllSameWidth) {
    throw std::out_of_range("Not all rows have same length");
  }

  ymir::Map<char, int> Map(Width, Height);
  ymir::Point2d<int> Pos = {0, 0};
  for (const auto &Row : MapRepr) {
    for (const char Char : Row) {
      Map.setTile(Pos, Char);
      Pos.X++;
    }
    Pos.X = 0;
    Pos.Y++;
  }
  return Map;
}

namespace {

std::vector<std::string> loadLines(const std::filesystem::path &Path) {
  std::ifstream FileStream(Path.c_str(), std::ios::in);
  if (!FileStream.is_open()) {
    throw std::runtime_error("Could not open " + Path.string() +
                             " for reading");
  }
  std::string Line;
  std::vector<std::string> Lines;
  while (std::getline(FileStream, Line)) {
    if (Line.empty()) {
      continue;
    }
    Lines.push_back(Line);
  }
  return Lines;
}

} // namespace

ymir::Map<char, int> loadMap(const std::filesystem::path &Path) {
  auto Lines = loadLines(Path);
  return loadMap(Lines);
}

} // namespace ymir