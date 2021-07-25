#ifndef YMIR_DUNGEON_BUILDER_HPP
#define YMIR_DUNGEON_BUILDER_HPP

#include <vector>
#include <ymir/Dungeon/Room.hpp>
#include <ymir/Map.hpp>

namespace ymir {

template <typename T, typename RE, typename U = int> class DungeonBuilder {
public:
  DungeonBuilder(Size2d<U> Size, RE &RndEng) : M(Size), RndEng(RndEng) {}

  // FIXME use pointer or reference
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

  void generateLoops(T Ground, T Wall) {
    (void)Ground;
    (void)Wall;
    for (auto const &Room : Rooms) {
      for (auto const &Door : Room.Doors) {
        (void)Door;
      }
      (void)Room;
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

    for (const auto &Room : Rooms) {
      M.merge(Room.M, Room.Pos);
    }

    for (const auto &Hallway : Hallways) {
      M.fillRect(Ground, Hallway);
    }
  }

  bool doesRoomAndHallwayFit(const Dungeon::Room<T, U> &NewRoom,
                             Rect2d<U> Hallway,
                             const Dungeon::Room<T, U> &TargetRoom) const {
    // Check that room and hallway are contained in map
    if (!M.rect().contains(NewRoom.rect()) || !M.rect().contains(Hallway)) {
      return false;
    }

    // Check that neither the hallway nor the room overlap with other rooms,
    // except that the hallway is allowed to overlap with the target room
    bool RoomOverlaps = std::any_of(
        Rooms.begin(), Rooms.end(),
        [&NewRoom, &Hallway,
         &TargetRoom](const Dungeon::Room<T, U> &OtherRoom) {
          auto RoomOverlap = OtherRoom.rect() & NewRoom.rect();
          auto HallwayOverlap = OtherRoom.rect() & Hallway;
          return !RoomOverlap.empty() ||
                 (!HallwayOverlap.empty() && &OtherRoom != &TargetRoom);
        });

    // Check if the room overlaps with another hallway, it does not matter if
    // the hallway overlaps
    bool HallwayOverlaps = std::any_of(
        Hallways.begin(), Hallways.end(), [&NewRoom](const Rect2d<U> &Other) {
          return !(Other & NewRoom.rect()).empty();
        });

    return !RoomOverlaps && !HallwayOverlaps;
  }

  bool tryToInsertRoom(Dungeon::Room<T, U> &NewRoom, Dungeon::Door<U> &RoomDoor,
                       const Dungeon::Room<T, U> &TargetRoom,
                       Dungeon::Door<U> &Door) {

    // get position for alignment
    ymir::Point2d<U> AlignmentPos = TargetRoom.Pos + Door.Pos + Door.Dir;
    for (; M.contains(AlignmentPos); AlignmentPos += Door.Dir) {

      // Calculate the new room position for the next alignment
      NewRoom.Pos = AlignmentPos - RoomDoor.Pos;

      // Create hallway between target and new room
      auto TargetDoorPos = TargetRoom.Pos + Door.Pos;
      auto NewDoorPos = NewRoom.Pos + RoomDoor.Pos;
      auto Hallway = Rect2d<U>::get(TargetDoorPos, NewDoorPos);
      Hallway.Size += Size2d<U>(1, 1);

      // check if room overlaps or hallway overlaps
      if (!doesRoomAndHallwayFit(NewRoom, Hallway, TargetRoom)) {
        continue;
      }

      Door.Used = true;
      RoomDoor.Used = true;
      Rooms.push_back(NewRoom); // FIXME this copies
      Hallways.push_back(Hallway);
      return true;
    }
    return false;
  }

  const ymir::Map<T, U> &getMap() const { return M; }
  const std::vector<Dungeon::Room<T, U>> &getRooms() const { return Rooms; }
  const std::vector<Rect2d<U>> &getHallways() const { return Hallways; }

private:
  ymir::Map<T, U> M;
  RE &RndEng;

  std::vector<Dungeon::Room<T, U>> Rooms;
  std::vector<Rect2d<U>> Hallways;
};

} // namespace ymir

#endif // #ifndef YMIR_DUNGEON_BUILDER_HPP
