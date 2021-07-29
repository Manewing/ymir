#ifndef YMIR_DUNGEON_DOOR_HPP
#define YMIR_DUNGEON_DOOR_HPP

#include <iostream>
#include <ymir/Dungeon/Object.hpp>

namespace ymir::Dungeon {

template <typename U> struct Door : public Object<U> {
  // FIXME either make class obsolete or add required additional attributes
  using Object<U>::Object;
  Door(Point2d<U> Pos, Dir2d Dir, bool Used = false)
      : Object<U>{Pos, Dir, Used} {}
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