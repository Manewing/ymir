#ifndef YMIR_ROOM_HPP
#define YMIR_ROOM_HPP

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>
#include <ymir/DebugTile.hpp>
#include <ymir/Dungeon/Door.hpp>
#include <ymir/Map.hpp>
#include <ymir/MapFilter.hpp>
#include <ymir/Noise.hpp>
#include <ymir/Types.hpp>

namespace ymir::Dungeon {

template <typename T, typename U> struct Room {
  Map<T, U> M;
  std::vector<Door<U>> Doors;
  Point2d<U> Pos = {0, 0};

  Door<U>* getDoor(Dir2d Dir) {
    auto It =
        std::find_if(Doors.begin(), Doors.end(), [Dir](const Door<U> &Door) {
          return (Dir & Door.Dir) != Dir2d::NONE;
        });
    if (It == Doors.end()) {
      return nullptr;
    }
    return &*It;
  }

  Dir2d getDoorDirections() const {
    Dir2d Dirs = Dir2d::NONE;
    std::for_each(Doors.begin(), Doors.end(),
                  [&Dirs](const Door<U> &Door) { Dirs |= Door.Dir; });
    return Dirs;
  }

  unsigned getNumUsedDoors() const {
    return std::count_if(Doors.begin(), Doors.end(),
                         [](const Door<U> &Door) { return Door.Used; });
  }

  bool blocksDoor(Point2d<U> Pos, bool Used = true) const {
    return std::any_of(Doors.begin(), Doors.end(),
                       [Pos, Used](const Door<U> &Door) {
                         return Used == Door.Used && Door.isBlockedBy(Pos);
                       });
  }

  Dir2d getOpposingDoorDirections() const {
    Dir2d Dirs = Dir2d::NONE;
    std::for_each(Doors.begin(), Doors.end(), [&Dirs](const Door<U> &Door) {
      Dirs |= Door.Dir.opposing();
    });
    return Dirs;
  }

  Rect2d<U> rect() const { return {Pos, M.Size}; }
};

namespace internal {
template <typename T, typename U>
void markDoors(ymir::Map<T, U> &M, const std::vector<Door<U>> &Doors,
               Point2d<U> Offset = {0, 0}) {
  for (const auto &Door : Doors) {
    switch (Door.Dir) {
    case Dir2d::DOWN:
      M.getTile(Offset + Door.Pos) = 'v';
      break;
    case Dir2d::UP:
      M.getTile(Offset + Door.Pos) = '^';
      break;
    case Dir2d::RIGHT:
      M.getTile(Offset + Door.Pos) = '>';
      break;
    case Dir2d::LEFT:
      M.getTile(Offset + Door.Pos) = '<';
      break;
    default:
      break;
    }
  }
}
} // namespace internal

template <typename T, typename U>
std::ostream &operator<<(std::ostream &Out, const Room<T, U> &Room) {
  Out << "Room{" << Room.Pos << ", /*Doors=*/{";
  const char *Separator = "";
  for (const auto &Door : Room.Doors) {
    Out << Separator << Door;
    Separator = ", ";
  }
  Out << "}}: " << std::endl;
  auto MapCopy = DebugTile<T, char>::convert(Room.M);
  internal::markDoors(MapCopy, Room.Doors);
  Out << MapCopy << std::endl;
  return Out;
}

template <typename T, typename U>
std::vector<Door<U>> getDoorCandidates(Map<T, U> &Room, T Ground) {
  std::vector<Door<U>> Doors;
  Room.forEach([&Doors, &Room, Ground](Point2d<U> P, T &Tile) {
    if (Tile == Ground) {
      return;
    }
    auto Count = Room.getNeighborCount(P, Ground);
    if (Count != 3) {
      return;
    }
    auto VEdge = verticalEdgeFilter(Room, P, Ground);
    auto HEdge = horizontalEdgeFilter(Room, P, Ground);
    if (VEdge == 3) {
      Doors.push_back({P, Dir2d::DOWN});
    } else if (VEdge == -3) {
      Doors.push_back({P, Dir2d::UP});
    } else if (HEdge == 3) {
      Doors.push_back({P, Dir2d::RIGHT});
    } else if (HEdge == -3) {
      Doors.push_back({P, Dir2d::LEFT});
    }
  });
  return Doors;
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_ROOM_HPP