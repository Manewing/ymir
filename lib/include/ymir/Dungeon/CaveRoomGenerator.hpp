#ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP

#include <ymir/Dungeon/RoomGenerator.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RndEngType>
class CaveRoomGenerator : public RoomGenerator<TileType, TileCord, RndEngType> {
public:
  using RoomGenerator<TileType, TileCord, RndEngType>::getCtx;

  static const char *Name;

public:
  CaveRoomGenerator() = default;
  const char *getName() const override { return Name; }

  Room<TileType, TileCord> generate() override;
};

template <typename T, typename U, typename RE>
const char *CaveRoomGenerator<T, U, RE>::Name = "cave_room_generator";

template <typename T, typename U, typename RE>
Room<T, U> CaveRoomGenerator<T, U, RE>::generate() {
  // FIXME make this configurable
  const Rect2d<U> RoomMinMax = {{5, 5}, {10, 10}};
  for (int Attempts = 0; Attempts < 100; Attempts++) {
    const auto RoomSize = randomSize2d<U>(RoomMinMax, getCtx().RndEng);
    auto RoomMap = generateCaveRoom(getCtx().Ground, getCtx().Wall, RoomSize,
                                    getCtx().RndEng);
    auto RoomDoors = getDoorCandidates(RoomMap, getCtx().Ground);
    if (!RoomDoors.empty()) {
      return {std::move(RoomMap), std::move(RoomDoors)};
    }
  }
  throw std::runtime_error("Could not generate new room");
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP