#ifndef YMIR_DUNGEON_HALLWAY_H
#define YMIR_DUNGEON_HALLWAY_H

#include <ymir/Dungeon/Room.hpp>

namespace ymir::Dungeon {

template <typename T, typename U>
struct Hallway {
  Rect2d<U> Rect;
  const Room<T, U> *Src;
  const Room<T, U> *Dst;
};

}

#endif // #ifndef YMIR_DUNGEON_HALLWAY_H