#ifndef YMIR_DUNGEON_HALLWAY_H
#define YMIR_DUNGEON_HALLWAY_H

#include <ymir/Dungeon/Room.hpp>

namespace ymir::Dungeon {

template <typename T, typename U> struct Hallway {
  Rect2d<U> Rect;
  Room<T, U> *Src = nullptr;
  Door<U> *SrcDoor = nullptr;
  Room<T, U> *Dst = nullptr;
  Door<U> *DstDoor = nullptr;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_HALLWAY_H