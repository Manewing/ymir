#ifndef YMIR_ROOM_HPP
#define YMIR_ROOM_HPP

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>
#include <ymir/CallularAutomata.hpp>
#include <ymir/Map.hpp>
#include <ymir/MapFilter.hpp>
#include <ymir/Noise.hpp>
#include <ymir/Types.hpp>

namespace ymir {

template <typename U> struct DungeonDoor {
  Point2d<U> Pos;
  Dir2d Dir = Dir2d::NONE;
  bool Used = false;
};

template <typename U>
std::ostream &operator<<(std::ostream &Out, const DungeonDoor<U> &Door) {
  Out << "DungeonDoor{" << Door.Pos << ", 0x" << std::hex << (int)Door.Dir
      << "}" << std::dec; // FIXME prevent cast here
  return Out;
}

template <typename T, typename U> struct DungeonRoom {
  Map<T, U> M;
  std::vector<DungeonDoor<U>> Doors;
  Point2d<U> Pos = {0, 0};

  DungeonDoor<U> *getDoor(Dir2d Dir) {
    auto It = std::find_if(
        Doors.begin(), Doors.end(),
        [Dir](const ymir::DungeonDoor<U> &Door) { return Dir == Door.Dir; });
    if (It == Doors.end()) {
      return nullptr;
    }
    return &*It;
  }

  Dir2d getDoorDirections() const {
    Dir2d Dirs = Dir2d::NONE;
    std::for_each(Doors.begin(), Doors.end(),
                  [&Dirs](const DungeonDoor<U> &Door) { Dirs |= Door.Dir; });
    return Dirs;
  }

  Dir2d getOpposingDoorDirections() const {
    Dir2d Dirs = Dir2d::NONE;
    std::for_each(
        Doors.begin(), Doors.end(),
        [&Dirs](const DungeonDoor<U> &Door) { Dirs |= Door.Dir.opposing(); });
    return Dirs;
  }

  Rect2d<U> rect() const { return {Pos, M.Size}; }
};

// TODO std::ostream& DungeonRoom

template <typename T, typename U>
std::vector<DungeonDoor<U>> getDoorCandidates(Map<T, U> &Room, T Ground) {
  std::vector<DungeonDoor<U>> Doors;
  Room.forEach([&Doors, &Room, Ground](Point2d<U> P, T &Tile) {
    auto Count = Room.getNeighborCount(P, Ground);
    if (Count == 3 && Tile != Ground) {
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
    }
  });
  return Doors;
}

template <typename T, typename U, typename RE>
std::vector<ymir::DungeonDoor<U>> getDoors(ymir::Map<T, U> &Room, T Ground,
                                           RE &RndEng) {
  std::vector<ymir::DungeonDoor<U>> Doors;
  auto DoorCandidates = getDoorCandidates(Room, Ground);

  char RoomsExisting = 0;

  Doors = DoorCandidates;
  (void)RndEng;
  (void)RoomsExisting;
  //const float DoorReplaceChance = 0.25f;
  //std::uniform_real_distribution<float> Uni(0, 1);
  //for (const auto &[P, Dir, Used] : DoorCandidates) {
  //  (void)Used;
  //  if (Dir & RoomsExisting) {
  //    if (Uni(RndEng) < DoorReplaceChance) {
  //      std::replace_if(
  //          Doors.begin(), Doors.end(),
  //          [Dir](const ymir::DungeonDoor<U> &Door) { return Door.Dir == Dir; },
  //          ymir::DungeonDoor<U>{P, Dir});
  //      continue;
  //    } else {
  //      continue;
  //    }
  //  }

  //  RoomsExisting |= Dir;
  //  Doors.push_back({P, Dir});
  //}

  //for (const auto &Door : Doors) {
  //  switch (Door.Dir) {
  //  case Dir2d::DOWN:
  //    Room.setTile(Door.Pos, 'v');
  //    break;
  //  case Dir2d::UP:
  //    Room.setTile(Door.Pos, '^');
  //    break;
  //  case Dir2d::RIGHT:
  //    Room.setTile(Door.Pos, '>');
  //    break;
  //  case Dir2d::LEFT:
  //    Room.setTile(Door.Pos, '<');
  //    break;
  //  default:
  //    break;
  //  }
  //}

  return Doors;
}

template <typename U, typename T, typename RE>
Map<T, U> generateMultiRectRoom(T Ground, T Wall, Size2d<U> Size, RE &RndEng) {
  Map<T, U> Room(Size);
  Room.fillRect(Wall);

  for (int L = 0; L < 2; L++) {
    const auto SizeRange = Rect2d<U>{{3, 3}, {Size.W - 4, Size.H - 4}};
    const auto RandomSubSize = randomSize2d<U>(SizeRange, RndEng);

    const auto PosRange = Rect2d<U>{
        {1, 1}, {Size.W - RandomSubSize.W - 1, Size.H - RandomSubSize.H - 1}};
    const auto RandomSubPos = randomPoint2d<U>(PosRange, RndEng);

    const Rect2d<U> FirstRoom = {RandomSubPos, RandomSubSize};
    Room.fillRect(Ground, FirstRoom);
  }

  return Room;
}

template <typename U, typename T, typename RE>
Map<T, U> generateCaveRoom(T Ground, T Wall, Size2d<U> Size, RE &RndEng) {
  Map<T, U> Room(Size);
  Room.fillRect(Wall);

  // Generate initial ground tiles
  const float GroundChance = 0.85f;
  fillRectRandom(Room, Ground, GroundChance, RndEng,
                 Rect2d<U>{{1, 1}, {Size.W - 2, Size.H - 2}});

  // Run replacement
  const std::size_t ReplaceThres = 4, Iterations = 6;
  for (std::size_t Idx = 0; Idx < Iterations; Idx++) {
    celat::replace(Room, Ground, Wall, ReplaceThres);
  }

  // Smooth generation
  const std::size_t SmoothThres = 5;
  celat::generate(Room, Ground, SmoothThres);

  // Kill isolated ground
  celat::generate(Room, Wall, 8);

  return Room;
}

template <typename U, typename T, typename RE>
Map<T, U> generateRandomRoom(T Ground, T Wall, RE &RndEng,
                             Rect2d<U> RoomMinMax = {{5, 5}, {10, 10}}) {
  const auto RoomSize = ymir::randomSize2d<U>(RoomMinMax, RndEng);
  std::uniform_int_distribution Uni(0, 1);
  if (Uni(RndEng)) {
    return generateMultiRectRoom(Ground, Wall, RoomSize, RndEng);
  }
  return generateCaveRoom(Ground, Wall, RoomSize, RndEng);
}

} // namespace ymir

#endif // #ifndef YMIR_ROOM_HPP