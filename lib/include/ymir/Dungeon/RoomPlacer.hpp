#ifndef YMIR_DUNGEON_ROOM_PLACER_HPP
#define YMIR_DUNGEON_ROOM_PLACER_HPP

#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/Room.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RandEngType>
class RoomPlacer : public BuilderBase {
public:
  static const char *Name;

public:
  RoomPlacer() = default;
  const char *getName() const override { return Name; }

  void run(BuilderPass &Pass, BuilderContext &C) override;

private:
  std::vector<std::pair<Room<TileType, TileCord> *, Dungeon::Door<TileCord> *>>
  getSuitableDoors(const Room<TileType, TileCord> &NewRoom,
                   std::list<Room<TileType, TileCord>> &Rooms);
  Room<TileType, TileCord> getNewRoom(TileType Ground, TileType Wall,
                                      RandEngType &RE);
  Room<TileType, TileCord> generateInitialRoom(Map<TileType, TileCord> &M,
                                               TileType Ground, TileType Wall,
                                               RandEngType &RE);

  bool tryToInsertRoom(Context<TileType, TileCord, RandEngType> &Ctx,
                       Dungeon::Room<TileType, TileCord> &NewRoom,
                       Dungeon::Door<TileCord> &RoomDoor,
                       Dungeon::Room<TileType, TileCord> &TargetRoom,
                       Dungeon::Door<TileCord> &Door);
};

template <typename T, typename U, typename RE>
const char *RoomPlacer<T, U, RE>::Name = "room_placer";

template <typename T, typename U, typename RE>
std::vector<std::pair<Dungeon::Room<T, U> *, Dungeon::Door<U> *>>
RoomPlacer<T, U, RE>::getSuitableDoors(const Dungeon::Room<T, U> &NewRoom,
                                       std::list<Room<T, U>> &Rooms) {
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

template <typename T, typename U, typename RE>
Dungeon::Room<T, U> RoomPlacer<T, U, RE>::getNewRoom(T Ground, T Wall,
                                                     RE &RndEng) {
  for (int Attempts = 0; Attempts < 100; Attempts++) {
    auto RoomMap = Dungeon::generateRandomRoom<U>(Ground, Wall, RndEng);
    auto RoomDoors = Dungeon::getDoorCandidates(RoomMap, Ground);
    if (!RoomDoors.empty()) {
      return {std::move(RoomMap), std::move(RoomDoors)};
    }
  }
  throw std::runtime_error("Could not generate new room");
}

template <typename T, typename U, typename RE>
Room<T, U> RoomPlacer<T, U, RE>::generateInitialRoom(Map<T, U> &M, T Ground,
                                                     T Wall, RE &RndEng) {
  auto NewRoom = getNewRoom(Ground, Wall, RndEng);
  const auto Size = NewRoom.M.Size;
  const auto PosRange =
      ymir::Rect2d<U>{{1, 1}, {M.Size.W - Size.W - 1, M.Size.H - Size.H - 1}};
  NewRoom.Pos = ymir::randomPoint2d<U>(PosRange, RndEng);
  return NewRoom;
}

template <typename T, typename U, typename RE>
void RoomPlacer<T, U, RE>::run(BuilderPass & /*Pass*/, BuilderContext &C) {
  auto &Ctx = C.get<Context<T, U, RE>>();

  // FIXME
  auto Wall = Ctx.Wall;
  auto Ground = Ctx.Ground;
  auto &Rooms = Ctx.Rooms;

  // FIXME make configurable
  unsigned NewRoomAttempts = 30;

  // Make entire map walls
  Ctx.M.fillRect(Wall); // FIXME this should not be done by the placer

  // Create initial room
  auto InitialRoom = generateInitialRoom(Ctx.M, Ground, Wall, Ctx.RndEng);
  Ctx.Rooms.push_back(std::move(InitialRoom));

  do {
    ymir::Dungeon::Room<T, U> NewRoom = getNewRoom(Ground, Wall, Ctx.RndEng);

    // Select a suitable room and door randomly from all available
    auto SuitableDoors = getSuitableDoors(NewRoom, Ctx.Rooms);
    if (SuitableDoors.empty()) {
      continue;
    }
    std::shuffle(SuitableDoors.begin(), SuitableDoors.end(), Ctx.RndEng);

    // Iterate over all suitable doors and try to insert the room into the
    // dungeon with a hallway starting from the door
    bool Inserted = false;
    while (!SuitableDoors.empty() && !Inserted) {
      auto [TargetRoom, Door] = SuitableDoors.back();
      SuitableDoors.pop_back();

      // get matching door
      auto RoomDoor = NewRoom.getDoor(Door->Dir.opposing());
      assert(RoomDoor && "No room door found foor suitable door");

      Inserted = tryToInsertRoom(Ctx, NewRoom, *RoomDoor, *TargetRoom, *Door);
    }
  } while (--NewRoomAttempts);

  // FIXME create a finalize method or sth?
  for (const auto &Room : Rooms) {
    Ctx.M.merge(Room.M, Room.Pos);
  }

  for (const auto &Hallway : Ctx.Hallways) {
    Ctx.M.fillRect(Ground, Hallway.Rect);
  }
}

template <typename T, typename U, typename RE>
bool RoomPlacer<T, U, RE>::tryToInsertRoom(Context<T, U, RE> &Ctx,
                                           Dungeon::Room<T, U> &NewRoom,
                                           Dungeon::Door<U> &RoomDoor,
                                           Dungeon::Room<T, U> &TargetRoom,
                                           Dungeon::Door<U> &Door) {

  // get position for alignment
  ymir::Point2d<U> AlignmentPos = TargetRoom.Pos + Door.Pos + Door.Dir;
  for (; Ctx.M.contains(AlignmentPos); AlignmentPos += Door.Dir) {

    // Calculate the new room position for the next alignment
    NewRoom.Pos = AlignmentPos - RoomDoor.Pos;

    // Create hallway between target and new room
    auto TargetDoorPos = TargetRoom.Pos + Door.Pos;
    auto NewDoorPos = NewRoom.Pos + RoomDoor.Pos;
    auto HallwayRect =
        Context<T, U, RE>::getHallwayRect(TargetDoorPos, NewDoorPos);

    // check if room overlaps or hallway overlaps
    if (!Ctx.doesRoomAndHallwayFit(NewRoom, HallwayRect, TargetRoom)) {
      continue;
    }

    Door.Used = true;
    RoomDoor.Used = true;
    Ctx.Rooms.push_back(NewRoom); // FIXME this copies
    Ctx.Hallways.push_back(Dungeon::Hallway<T, U>{
        HallwayRect, &Ctx.Rooms.back(), &RoomDoor, &TargetRoom, &Door});
    return true;
  }
  return false;
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_ROOM_PLACER_HPP