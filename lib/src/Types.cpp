#include <sstream>
#include <tuple>
#include <ymir/Types.hpp>

namespace ymir {

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
  const std::array<std::pair<Dir2dValue, const char *>, 4> Strs = {
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