#ifndef YMIR_TEST_HELPERS_HPP
#define YMIR_TEST_HELPERS_HPP

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <ymir/Dungeon/Door.hpp>
#include <ymir/Dungeon/Room.hpp>
#include <ymir/Map.hpp>
#include <ymir/MapIo.hpp>

namespace {

template <typename Type>[[maybe_unused]] std::string dump(const Type &T) {
  std::stringstream SS;
  SS << T;
  return SS.str();
}

[[maybe_unused]] std::optional<ymir::Point2d<int>>
getMarkerPos(ymir::Map<char, int> &Map, char Marker,
             std::optional<char> Replace = std::nullopt) {
  std::optional<ymir::Point2d<int>> MarkerPos;
  Map.forEach([&MarkerPos, &Replace, Marker](auto Pos, char &Tile) {
    if (Tile == Marker) {
      MarkerPos = Pos;
      if (Replace) {
        Tile = *Replace;
      }
    }
  });
  return MarkerPos;
}

[[maybe_unused]] void markPath(const std::vector<ymir::Point2d<int>> &Path,
                               ymir::Map<char, int> &Map, char Marker = 'x') {
  for (const auto &Pos : Path) {
    Map.setTile(Pos, Marker);
  }
}

[[maybe_unused]] ymir::Dir2d getDir2d(char Char) {
  switch (Char) {
  case 'v':
    return ymir::Dir2d::DOWN;
  case '^':
    return ymir::Dir2d::UP;
  case '<':
    return ymir::Dir2d::LEFT;
  case '>':
    return ymir::Dir2d::RIGHT;
  default:
    break;
  }
  return ymir::Dir2d::NONE;
}

[[maybe_unused]] ymir::Dungeon::Room<char, int>
getRoom(const std::vector<std::string> &RoomRepr) {
  ymir::Map<char, int> M = ymir::loadMap(RoomRepr);
  ymir::Dungeon::Room<char, int> R{M, {}};
  R.M.forEach([&R](ymir::Point2d<int> Pos, char Char) {
    if (auto Dir = getDir2d(Char); Dir != ymir::Dir2d::NONE) {
      R.Doors.push_back(ymir::Dungeon::Door<int>{Pos, Dir, /*Used=*/false});
    }
  });
  return R;
}

#define EXPECT_MAP_EQ(Map, MapRef)                                             \
  EXPECT_EQ(Map, MapRef) << "Map:\n" << Map << "\nMap Ref:\n" << MapRef << "\n";

} // namespace

#endif // #ifndef YMIR_TEST_HELPERS_HPP
