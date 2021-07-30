#ifndef YMIR_DUNGEON_BUILDER_HPP
#define YMIR_DUNGEON_BUILDER_HPP

#include <list>
#include <vector>
#include <ymir/Dungeon/Hallway.hpp>
#include <ymir/Dungeon/Room.hpp>
#include <ymir/Map.hpp>

namespace ymir {

template <typename U>
bool checkIfOpposing(Point2d<U> SrcPos, Dir2d SrcDir, Point2d<U> TgtPos,
                     Dir2d TgtDir) {
  if (SrcDir.opposing() != TgtDir) {
    return false;
  }
  switch (SrcDir) {
  case Dir2d::LEFT:
    return SrcPos.X >= TgtPos.X;
  case Dir2d::RIGHT:
    return SrcPos.X <= TgtPos.X;
  case Dir2d::UP:
    return SrcPos.Y >= TgtPos.Y;
  case Dir2d::DOWN:
    return SrcPos.Y <= TgtPos.Y;
  default:
    break;
  }
  return false;
}

// TODO refactor split:
// - Dungeon::Context
// - function/class for generating rooms
// - function/class for generating loops
template <typename T, typename RE, typename U = int> class DungeonBuilder {
public:
  DungeonBuilder(Size2d<U> Size, RE &RndEng) : M(Size), RndEng(RndEng) {}

  // FIXME add rooom pointer to door?
  std::vector<std::pair<Dungeon::Room<T, U> *, Dungeon::Door<U> *>>
  getSuitableDoors(const Dungeon::Room<T, U> &NewRoom) {
    // TODO exclude doors that are already connected
    const auto Dirs = NewRoom.getOpposingDoorDirections();
    std::vector<std::pair<Dungeon::Room<T, U> *, Dungeon::Door<U> *>> Doors;
    for (auto &Room : Rooms) {
      for (auto &Door : Room.Doors) {
        // Check if door direction is suitable if so add door
        if ((Door.Dir & Dirs) && !Door.Used) {
          Doors.push_back({&Room, &Door});
        }
      }
    }
    return Doors;
  }

  Dungeon::Room<T, U> getNewRoom(T Ground, T Wall) {
    for (int Attempts = 0; Attempts < 100; Attempts++) {
      auto RoomMap = Dungeon::generateRandomRoom<U>(Ground, Wall, RndEng);
      auto RoomDoors = Dungeon::getDoorCandidates(RoomMap, Ground);
      if (!RoomDoors.empty()) {
        return {std::move(RoomMap), std::move(RoomDoors)};
      }
    }
    throw std::runtime_error("Could not generate new room");
  }

  bool haveRoomsHallway(const Dungeon::Room<T, U> &A,
                        const Dungeon::Room<T, U> &B) const {
    return std::any_of(Hallways.begin(), Hallways.end(),
                       [&A, &B](const Dungeon::Hallway<T, U> &Hallway) {
                         return (Hallway.Src == &A && Hallway.Dst == &B) ||
                                (Hallway.Src == &B && Hallway.Dst == &A);
                       });
  }

  std::optional<Dungeon::Hallway<T, U>>
  getLoopHallway(Dungeon::Room<T, U> &Source, Dungeon::Room<T, U> &Target) {
    std::vector<Dungeon::Hallway<T, U>> LoopHallways;
    for (auto &SrcDoor : Source.Doors) {
      for (auto &TgtDoor : Target.Doors) {
        auto SrcPos = Source.Pos + SrcDoor.Pos;
        auto TgtPos = Target.Pos + TgtDoor.Pos;
        if (!checkIfOpposing(SrcPos, SrcDoor.Dir, TgtPos, TgtDoor.Dir) ||
            SrcDoor.Used || TgtDoor.Used) {
          continue;
        }
        if ((SrcPos.X == TgtPos.X && SrcDoor.Dir.isVertical()) ||
            (SrcPos.Y == TgtPos.Y && SrcDoor.Dir.isHorizontal())) {
          auto HallwayRect = getHallwayRect(SrcPos, TgtPos);
          if (doesHallwayFit(HallwayRect, &Target, &Source)) {
            LoopHallways.push_back(Dungeon::Hallway<T, U>{
                HallwayRect, &Source, &SrcDoor, &Target, &TgtDoor});
          }
        }
      }
    }
    if (LoopHallways.empty()) {
      return std::nullopt;
    }
    return *randomIterator(LoopHallways.begin(), LoopHallways.end(), RndEng);
  }

  void generateLoops(unsigned MaxLoops, unsigned MaxUsedDoors) {
    std::vector<Dungeon::Hallway<T, U>> LoopHallways;
    for (auto It = Rooms.begin(); It != Rooms.end(); It++) {
      if (It->getNumUsedDoors() >= MaxUsedDoors) {
        continue;
      }
      for (auto NextIt = It; NextIt != Rooms.end(); NextIt++) {
        if (It == NextIt || haveRoomsHallway(*It, *NextIt) ||
            NextIt->getNumUsedDoors() >= MaxUsedDoors) {
          continue;
        }
        auto LoopHallway = getLoopHallway(*It, *NextIt);
        if (LoopHallway) {
          LoopHallways.push_back(*LoopHallway);
        }
      }
    }
    std::shuffle(LoopHallways.begin(), LoopHallways.end(), RndEng);
    for (std::size_t Idx = 0; Idx < MaxLoops && Idx < LoopHallways.size();
         Idx++) {
      auto &Hallway = LoopHallways.at(Idx);
      Hallway.SrcDoor->Used = true;
      Hallway.DstDoor->Used = true;
      Hallways.push_back(Hallway);
    }
  }

  void generateInitialRoom(T Ground, T Wall) {
    auto NewRoom = getNewRoom(Ground, Wall);
    const auto Size = NewRoom.M.Size;
    const auto PosRange =
        ymir::Rect2d<U>{{1, 1}, {M.Size.W - Size.W - 1, M.Size.H - Size.H - 1}};
    NewRoom.Pos = ymir::randomPoint2d<U>(PosRange, RndEng);
    Rooms.push_back(std::move(NewRoom));
  }

  void generate(T Ground, T Wall, unsigned NewRoomAttempts) {
    // Make entire map walls
    M.fillRect(Wall);

    // Create initial room
    generateInitialRoom(Ground, Wall);

    do {
      ymir::Dungeon::Room<T, U> NewRoom = getNewRoom(Ground, Wall);

      // Select a suitable room and door randomly from all available
      auto SuitableDoors = getSuitableDoors(NewRoom);
      if (SuitableDoors.empty()) {
        continue;
      }
      std::shuffle(SuitableDoors.begin(), SuitableDoors.end(), RndEng);

      // Iterate over all suitable doors and try to insert the room into the
      // dungeon with a hallway starting from the door
      bool Inserted = false;
      while (!SuitableDoors.empty() && !Inserted) {
        auto [TargetRoom, Door] = SuitableDoors.back();
        SuitableDoors.pop_back();

        // get matching door
        auto RoomDoor = NewRoom.getDoor(Door->Dir.opposing());
        assert(RoomDoor && "No room door found foor suitable door");

        Inserted = tryToInsertRoom(NewRoom, *RoomDoor, *TargetRoom, *Door);
      }
    } while (--NewRoomAttempts);

    // Generate loops between rooms, currently dungeon is a tree
    generateLoops(Rooms.size() / 2, 5);

    for (const auto &Room : Rooms) {
      M.merge(Room.M, Room.Pos);
    }

    for (const auto &Hallway : Hallways) {
      M.fillRect(Ground, Hallway.Rect);
    }
  }

  bool doesRoomFit(const Dungeon::Room<T, U> &Room) const {
    // If it's not contained in the map, it does not fit
    if (!M.rect().contains(Room.rect())) {
      return false;
    }

    // Check that the room neiter overlaps with another room nor with a hallway
    bool RoomOverlapsRoom =
        std::any_of(Rooms.begin(), Rooms.end(),
                    [&Room](const Dungeon::Room<T, U> &OtherRoom) {
                      auto Overlap = OtherRoom.rect() & Room.rect();
                      return !Overlap.empty();
                    });
    bool RoopmOverlapsHallway =
        std::any_of(Hallways.begin(), Hallways.end(),
                    [&Room](const Dungeon::Hallway<T, U> &Hallway) {
                      auto Overlap = Hallway.Rect & Room.rect();
                      return !Overlap.empty();
                    });

    return !RoomOverlapsRoom && !RoopmOverlapsHallway;
  }

  bool doesHallwayFit(Rect2d<U> HallwayRect,
                      const Dungeon::Room<T, U> *TargetRoom = nullptr,
                      const Dungeon::Room<T, U> *SourceRoom = nullptr) const {
    // If it's not contained in the map, it does not fit
    if (!M.rect().contains(HallwayRect)) {
      return false;
    }
    bool HallwayOverlapsRooms =
        std::any_of(Rooms.begin(), Rooms.end(),
                    [&HallwayRect, TargetRoom,
                     SourceRoom](const Dungeon::Room<T, U> &Room) {
                      if (&Room == TargetRoom || &Room == SourceRoom) {
                        return false;
                      }
                      auto HallwayOverlap = Room.rect() & HallwayRect;
                      return !HallwayOverlap.empty();
                    });
    return !HallwayOverlapsRooms;
  }

  bool doesRoomAndHallwayFit(const Dungeon::Room<T, U> &NewRoom,
                             Rect2d<U> HallwayRect,
                             const Dungeon::Room<T, U> &TargetRoom) const {
    return doesRoomFit(NewRoom) &&
           doesHallwayFit(HallwayRect, &TargetRoom, &NewRoom);
  }

  static Rect2d<U> getHallwayRect(Point2d<U> PosA, Point2d<U> PosB) {
    auto Hallway = Rect2d<U>::get(PosA, PosB);
    Hallway.Size += Size2d<U>(1, 1);
    return Hallway;
  }

  bool tryToInsertRoom(Dungeon::Room<T, U> &NewRoom, Dungeon::Door<U> &RoomDoor,
                       Dungeon::Room<T, U> &TargetRoom,
                       Dungeon::Door<U> &Door) {

    // get position for alignment
    ymir::Point2d<U> AlignmentPos = TargetRoom.Pos + Door.Pos + Door.Dir;
    for (; M.contains(AlignmentPos); AlignmentPos += Door.Dir) {

      // Calculate the new room position for the next alignment
      NewRoom.Pos = AlignmentPos - RoomDoor.Pos;

      // Create hallway between target and new room
      auto TargetDoorPos = TargetRoom.Pos + Door.Pos;
      auto NewDoorPos = NewRoom.Pos + RoomDoor.Pos;
      auto HallwayRect = getHallwayRect(TargetDoorPos, NewDoorPos);

      // check if room overlaps or hallway overlaps
      if (!doesRoomAndHallwayFit(NewRoom, HallwayRect, TargetRoom)) {
        continue;
      }

      Door.Used = true;
      RoomDoor.Used = true;
      Rooms.push_back(NewRoom); // FIXME this copies
      Hallways.push_back(Dungeon::Hallway<T, U>{HallwayRect, &Rooms.back(),
                                                &RoomDoor, &TargetRoom, &Door});
      return true;
    }
    return false;
  }

  ymir::Map<T, U> &getMap() { return M; }
  const ymir::Map<T, U> &getMap() const { return M; }
  const std::list<Dungeon::Room<T, U>> &getRooms() const { return Rooms; }
  const std::vector<Dungeon::Hallway<T, U>> &getHallways() const {
    return Hallways;
  }

private:
  ymir::Map<T, U> M;
  RE &RndEng;

  std::list<Dungeon::Room<T, U>> Rooms;
  std::vector<Dungeon::Hallway<T, U>> Hallways;
}; // namespace ymir


} // namespace ymir

#endif // #ifndef YMIR_DUNGEON_BUILDER_HPP
