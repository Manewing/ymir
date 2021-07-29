#ifndef YMIR_DUNGEON_OBJECT_HPP
#define YMIR_DUNGEON_OBJECT_HPP

#include <iostream>
#include <ymir/Types.hpp>

namespace ymir::Dungeon {

template <typename U> struct Object {
  Point2d<U> Pos;
  Dir2d Dir = Dir2d::NONE;
  bool Used = false;

  bool isBlockedBy(Point2d<U> P) const {
    return Pos == P || Dir.opposing() + Pos == P || Dir + Pos == P;
  }
};

template <typename U>
inline bool operator==(const Object<U> &Lhs, const Object<U> &Rhs) noexcept {
  return std::tie(Lhs.Pos, Lhs.Dir, Lhs.Used) ==
         std::tie(Rhs.Pos, Rhs.Dir, Rhs.Used);
}

template <typename U>
inline bool operator!=(const Object<U> &Lhs, const Object<U> &Rhs) noexcept {
  return !(Lhs == Rhs);
}

template <typename U>
std::ostream &operator<<(std::ostream &Out, const Object<U> &Object) {
  Out << "Object{" << Object.Pos << ", " << Object.Dir << ", /*Used=*/"
      << std::boolalpha << Object.Used << std::noboolalpha << "}";
  return Out;
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_OBJECT_HPP