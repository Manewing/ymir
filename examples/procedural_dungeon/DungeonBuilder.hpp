#ifndef YMIR_DUNGEON_BUILDER_HPP
#define YMIR_DUNGEON_BUILDER_HPP

#include <vector>
#include <ymir/Map.hpp>
#include <ymir/Room.hpp>

namespace ymir {

template <typename T, typename RE, typename U = int> class DungeonBuilder {
public:
  DungeonBuilder(Size2d<U> Size, RE &RndEng) : M(Size), RndEng(RndEng) {}

  // FIXME use pointer or reference
  // FIXME add rooom pointer to door?
  std::vector<std::pair<DungeonRoom<T, U> *, DungeonDoor<U> *>>
  getSuitableDoors(const DungeonRoom<T, U> &NewRoom) {
    // TODO exclude doors that are already connected
    const auto Dirs = NewRoom.getOpposingDoorDirections();
    std::vector<std::pair<DungeonRoom<T, U> *, DungeonDoor<U> *>> Doors;
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

  DungeonRoom<T, U> getNewRoom(T Ground, T Wall) {
    (void)Wall;
    for (int Attempts = 0; Attempts < 100; Attempts++) {
      auto RoomMap = generateRandomRoom<U>(Ground, '-', RndEng);
      auto RoomDoors = getDoors(RoomMap, Ground, RndEng);
      if (!RoomDoors.empty()) {
        return {std::move(RoomMap), std::move(RoomDoors)};
      }
    }
    throw std::runtime_error("Could not generate new room");
  }

  void generateLoops(T Ground, T Wall) {
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
    std::cerr << "initial room: " << std::endl << NewRoom.M << std::endl;
    Rooms.push_back(std::move(NewRoom));
  }

  void generate(T Ground, T Wall, unsigned NewRoomAttempts = 30) {
    // Make entire map walls
    M.fillRect(Wall);

    // Create initial room
    generateInitialRoom(Ground, Wall);

    do {
      ymir::DungeonRoom<T, U> NewRoom = getNewRoom(Ground, Wall);
      std::cerr << "new room: " << std::endl << NewRoom.M << std::endl;

      // Select a suitable room and door randomly from all available
      auto SuitableDoors = getSuitableDoors(NewRoom);
      if (SuitableDoors.empty()) {
        continue;
      }
      std::cerr << "found " << SuitableDoors.size() << " suitable doors"
                << std::endl;

      std::shuffle(SuitableDoors.begin(), SuitableDoors.end(), RndEng);

      bool Inserted = false;
      while (!SuitableDoors.empty() && !Inserted) {
        auto [TargetRoom, Door] = SuitableDoors.back();
        SuitableDoors.pop_back();
        //std::cerr << "picked: " << *Door << std::endl;

        // get matching door
        auto *RoomDoor = NewRoom.getDoor(Door->Dir.opposing());
        assert(RoomDoor && "No room door found foor suitable door");

        // DEBUG
        //std::cerr << "found: " << *RoomDoor << std::endl;
        //auto Tile = NewRoom.M.getTile(RoomDoor->Pos);
        //NewRoom.M.setTile(RoomDoor->Pos, '*');
        //std::cerr << NewRoom.M << std::endl;
        //NewRoom.M.setTile(RoomDoor->Pos, Tile);

        Inserted = tryToInsertRoom(NewRoom, *RoomDoor, *TargetRoom, *Door);
      }
    } while (--NewRoomAttempts);

    for (const auto &Room : Rooms) {
      M.merge(Room.M, Room.Pos);
    }

    for (const auto &Hallway : Hallways) {
      M.fillRect(Ground, Hallway);
    }

    for (const auto &Room : Rooms) {
      for (const auto &Door : Room.Doors) {
        if (!Door.Used) {
          continue;
        }
        switch (Door.Dir) {
        case Dir2d::DOWN:
          M.setTile(Room.Pos + Door.Pos, 'v');
          break;
        case Dir2d::UP:
          M.setTile(Room.Pos + Door.Pos, '^');
          break;
        case Dir2d::RIGHT:
          M.setTile(Room.Pos + Door.Pos, '>');
          break;
        case Dir2d::LEFT:
          M.setTile(Room.Pos + Door.Pos, '<');
          break;
        default:
          break;
        }
        (void)Door;
      }
    }
  }

  bool doesRoomAndHallwayFit(const DungeonRoom<T, U> &NewRoom,
                             Rect2d<U> Hallway,
                             const DungeonRoom<T, U> &TargetRoom) const {
    // Check that room and hallway are contained in map
    if (!M.rect().contains(NewRoom.rect()) || !M.rect().contains(Hallway)) {
      return false;
    }

    // Check that neither the hallway nor the room overlap with other rooms,
    // except that the hallway is allowed to overlap with the target room
    bool RoomOverlaps = std::any_of(
        Rooms.begin(), Rooms.end(),
        [&NewRoom, &Hallway, &TargetRoom](const DungeonRoom<T, U> &OtherRoom) {
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

  bool tryToInsertRoom(DungeonRoom<T, U> &NewRoom, DungeonDoor<U> &RoomDoor,
                       const DungeonRoom<T, U> &TargetRoom,
                       DungeonDoor<U> &Door) {

    // get position for alignment
    ymir::Point2d<U> AlignmentPos = TargetRoom.Pos + Door.Pos + Door.Dir;
    for (; M.contains(AlignmentPos); AlignmentPos += Door.Dir) {

      // Calculate the new room position for the next alignment
      NewRoom.Pos = AlignmentPos - RoomDoor.Pos;

      // Create hallway between target and new room
      auto Hallway = Rect2d<U>::get(TargetRoom.Pos + Door.Pos,
                                    NewRoom.Pos + RoomDoor.Pos);
      if (Door.Dir & Dir2d::VERTICAL) {
        Hallway.Size.W = Hallway.Size.W == 0 ? 1 : Hallway.Size.W;
      } else {
        Hallway.Size.H = Hallway.Size.H == 0 ? 1 : Hallway.Size.H;
      }
      //std::cerr << "  -> check hallway: " << Hallway
      //          << ", target: " << (TargetRoom.Pos + Door.Pos)
      //          << ", new: " << (NewRoom.Pos + RoomDoor.Pos) << std::endl;
      // M.fillRect('*', Hallway);

      // check if room overlaps or hallway overlaps
      if (!doesRoomAndHallwayFit(NewRoom, Hallway, TargetRoom)) {
        continue;
      }

      std::cerr << "insert room at " << NewRoom.Pos << std::endl;
      Door.Used = true;
      RoomDoor.Used = true;
      Rooms.push_back(NewRoom); // FIXME this copies
      Hallways.push_back(Hallway);
      return true;
    }
    return false;
  }

  const ymir::Map<T, U> &getMap() const { return M; }

private:
  ymir::Map<T, U> M;
  RE &RndEng;

  std::vector<DungeonRoom<T, U>> Rooms;
  std::vector<Rect2d<U>> Hallways;
};

} // namespace ymir

#endif // #ifndef YMIR_DUNGEON_BUILDER_HPP