#ifndef YMIR_TEST_HELPERS_HPP
#define YMIR_TEST_HELPERS_HPP

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <ymir/Dungeon/Door.hpp>
#include <ymir/Dungeon/Room.hpp>
#include <ymir/Map.hpp>

namespace {

template <typename Type>[[maybe_unused]] std::string dump(const Type &T) {
  std::stringstream SS;
  SS << T;
  return SS.str();
}

[[maybe_unused]] ymir::Map<char, int>
getMap(const std::vector<std::string> &MapRepr) {
  if (MapRepr.empty()) {
    return ymir::Map<char, int>(0, 0);
  }
  const auto Height = MapRepr.size();
  const auto Width = MapRepr.at(0).size();
  const auto AllSameWidth = std::all_of(
      MapRepr.begin(), MapRepr.end(),
      [Width](const std::string &Str) { return Str.size() == Width; });
  if (!AllSameWidth) {
    throw std::out_of_range("No all rows have same length");
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
  ymir::Map<char, int> M = getMap(RoomRepr);
  ymir::Dungeon::Room<char, int> R{M, {}};
  R.M.forEach([&R](ymir::Point2d<int> Pos, char Char) {
    if (auto Dir = getDir2d(Char); Dir != ymir::Dir2d::NONE) {
      R.Doors.push_back(ymir::Dungeon::Door<int>{Pos, Dir, /*Used=*/false});
    }
  });
  return R;
}

} // namespace

#endif // #ifndef YMIR_TEST_HELPERS_HPP
