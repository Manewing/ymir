#ifndef YMIR_DUNGEON_DOOR_HPP
#define YMIR_DUNGEON_DOOR_HPP

#include <iostream>
#include <ymir/Types.hpp>

namespace ymir::Dungeon {

template <typename U> struct Door {
  Point2d<U> Pos;
  Dir2d Dir = Dir2d::NONE;
  bool Used = false;
};

template <typename U>
inline bool operator==(const Door<U> &Lhs, const Door<U> &Rhs) noexcept {
  return std::tie(Lhs.Pos, Lhs.Dir, Lhs.Used) ==
         std::tie(Rhs.Pos, Rhs.Dir, Rhs.Used);
}

template <typename U>
inline bool operator!=(const Door<U> &Lhs, const Door<U> &Rhs) noexcept {
  return !(Lhs == Rhs);
}

template <typename U>
std::ostream &operator<<(std::ostream &Out, const Door<U> &Door) {
  Out << "Door{" << Door.Pos << ", " << Door.Dir << ", /*Used=*/"
      << std::boolalpha << Door.Used << std::noboolalpha << "}";
  return Out;
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_DOOR_HPP