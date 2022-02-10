#ifndef YMIR_DUNGEON_CONTEXT_HPP
#define YMIR_DUNGEON_CONTEXT_HPP

#include <list>
#include <vector>
#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/Hallway.hpp>
#include <ymir/Dungeon/Object.hpp>
#include <ymir/Dungeon/Room.hpp>
#include <ymir/LayeredMap.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord>
class Context : public BuilderContext {
public:
  static Rect2d<TileCord> getHallwayRect(Point2d<TileCord> PosA,
                                         Point2d<TileCord> PosB);

public:
  Context(LayeredMap<TileType, TileCord> &Map) : Map(Map) {}

  bool isInRoom(Point2d<TileCord> Pos) const;
  bool isInHallway(Point2d<TileCord> Pos) const;

  bool haveRoomsHallway(const Dungeon::Room<TileType, TileCord> &A,
                        const Dungeon::Room<TileType, TileCord> &B) const;

  bool doesRoomFit(const Dungeon::Room<TileType, TileCord> &Room) const;

  bool doesHallwayFit(
      Rect2d<TileCord> HallwayRect,
      const Dungeon::Room<TileType, TileCord> *TargetRoom = nullptr,
      const Dungeon::Room<TileType, TileCord> *SourceRoom = nullptr) const;

  bool doesRoomAndHallwayFit(
      const Dungeon::Room<TileType, TileCord> &NewRoom,
      Rect2d<TileCord> HallwayRect,
      const Dungeon::Room<TileType, TileCord> &TargetRoom) const;

public:
  LayeredMap<TileType, TileCord> &Map;
  std::list<Dungeon::Room<TileType, TileCord>> Rooms;
  std::vector<Dungeon::Hallway<TileType, TileCord>> Hallways;
};

// Returns true if position is inside room and the position is empty
template <typename T, typename U>
bool Context<T, U>::isInRoom(Point2d<U> Pos) const {
  return std::any_of(Rooms.begin(), Rooms.end(),
                     [&Pos](const Dungeon::Room<T, U> &Room) {
                       return Room.rect().contains(Pos) &&
                              Room.M.getTile(Pos - Room.Pos) == T();
                     });
}

template <typename T, typename U>
bool Context<T, U>::isInHallway(Point2d<U> Pos) const {
  return std::any_of(Hallways.begin(), Hallways.end(),
                     [&Pos](const Dungeon::Hallway<T, U> &Hallway) {
                       return Hallway.rect().contains(Pos);
                     });
}

template <typename T, typename U>
bool Context<T, U>::haveRoomsHallway(const Dungeon::Room<T, U> &A,
                                     const Dungeon::Room<T, U> &B) const {
  return std::any_of(Hallways.begin(), Hallways.end(),
                     [&A, &B](const Dungeon::Hallway<T, U> &Hallway) {
                       return (Hallway.Src == &A && Hallway.Dst == &B) ||
                              (Hallway.Src == &B && Hallway.Dst == &A);
                     });
}

template <typename T, typename U>
bool Context<T, U>::doesRoomFit(const Dungeon::Room<T, U> &Room) const {
  // If it's not contained in the map, it does not fit
  const auto RoomRect = Room.rect();
  if (!Map.rect().contains(RoomRect)) {
    return false;
  }

  // Check that the room neiter overlaps with another room nor with a hallway
  bool RoomOverlapsRoom =
      std::any_of(Rooms.begin(), Rooms.end(),
                  [RoomRect](const Dungeon::Room<T, U> &OtherRoom) {
                    return RoomRect.overlaps(OtherRoom.rect());
                  });
  bool RoopmOverlapsHallway =
      std::any_of(Hallways.begin(), Hallways.end(),
                  [RoomRect](const Dungeon::Hallway<T, U> &Hallway) {
                    return RoomRect.overlaps(Hallway.rect());
                  });

  return !RoomOverlapsRoom && !RoopmOverlapsHallway;
}

template <typename T, typename U>
bool Context<T, U>::doesHallwayFit(
    Rect2d<U> HallwayRect, const Dungeon::Room<T, U> *TargetRoom,
    const Dungeon::Room<T, U> *SourceRoom) const {
  // If it's not contained in the map, it does not fit
  if (!Map.rect().contains(HallwayRect)) {
    return false;
  }

  bool HallwayOverlapsRooms = std::any_of(
      Rooms.begin(), Rooms.end(),
      [&HallwayRect, TargetRoom, SourceRoom](const Dungeon::Room<T, U> &Room) {
        if (&Room == TargetRoom || &Room == SourceRoom) {
          return false;
        }
        return Room.rect().overlaps(HallwayRect);
      });

  return !HallwayOverlapsRooms;
}

template <typename T, typename U>
bool Context<T, U>::doesRoomAndHallwayFit(
    const Dungeon::Room<T, U> &NewRoom, Rect2d<U> HallwayRect,
    const Dungeon::Room<T, U> &TargetRoom) const {
  return doesRoomFit(NewRoom) &&
         doesHallwayFit(HallwayRect, &TargetRoom, &NewRoom);
}

template <typename T, typename U>
Rect2d<U> Context<T, U>::getHallwayRect(Point2d<U> PosA, Point2d<U> PosB) {
  auto Hallway = Rect2d<U>::get(PosA, PosB);
  Hallway.Size += Size2d<U>(1, 1);
  return Hallway;
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CONTEXT_HPP