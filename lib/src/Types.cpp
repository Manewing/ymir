#include <algorithm>
#include <iostream>
#include <sstream>
#include <tuple>
#include <array>
#include <ymir/Types.hpp>

namespace ymir {

Dir2d Dir2d::fromString(std::string Str) {
  Dir2d Dir = NONE;
  static const std::array<std::pair<Dir2dValue, const char *>, 6> Strs = {{
      {UP, "UP"},
      {DOWN, "DOWN"},
      {LEFT, "LEFT"},
      {RIGHT, "RIGHT"},
      {HORIZONTAL, "HORIZONTAL"},
      {VERTICAL, "VERTICAL"},
  }};

  std::replace(Str.begin(), Str.end(), '|', ' ');
  std::stringstream SS(Str);
  while (SS >> Str) {
    for (auto const &[Val, Name] : Strs) {
      if (Name == Str) {
        Dir |= Val;
      }
    }
  }
  return Dir;
}

Dir2d Dir2d::fromVector(int X, int Y) {
  if (X == 0 && Y == 0) {
    return NONE;
  } else if (X == 0) {
    return Y > 0 ? DOWN : UP;
  } else if (Y == 0) {
    return X > 0 ? RIGHT : LEFT;
  } else {
    return NONE;
  }
}

std::string Dir2d::str() const {
  switch (Value) {
  case NONE:
    return "NONE";
  case VERTICAL:
    return "VERTICAL";
  case HORIZONTAL:
    return "HORIZONTAL";
  default:
    break;
  }

  std::stringstream SS;
  static const std::array<std::pair<Dir2dValue, const char *>, 4> Strs = {
      {{UP, "UP"}, {DOWN, "DOWN"}, {LEFT, "LEFT"}, {RIGHT, "RIGHT"}}};

  const char *Separator = "";
  for (auto const &KV : Strs) {
    if (Value & KV.first) {
      SS << Separator << KV.second;
      Separator = " | ";
    }
  }
  return SS.str();
}

Dir2d &Dir2d::operator|=(Dir2d Other) {
  Value |= Other.Value;
  return *this;
}

Dir2d &Dir2d::operator&=(Dir2d Other) {
  Value &= Other.Value;
  return *this;
}

Dir2d &Dir2d::operator^=(Dir2d Other) {
  Value ^= Other.Value;
  return *this;
}

Dir2d Dir2d::opposing() const {
  switch (Value) {
  case NONE:
    return NONE;
  case DOWN:
    return UP;
  case UP:
    return DOWN;
  case RIGHT:
    return LEFT;
  case LEFT:
    return RIGHT;
  default:
    break;
  }
  return NONE;
}

std::ostream &operator<<(std::ostream &Out, const Dir2d &Dir) {
  Out << "Dir2d(" << Dir.str() << ")";
  return Out;
}

} // namespace ymir