#ifndef YMIR_DUNGEON_RECT_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_RECT_ROOM_GENERATOR_HPP

#include <ymir/Dungeon/RoomGenerator.hpp>

namespace ymir::Dungeon {

// TODO rename MultiRectRoomGenerator and add (single) RectRoomGenerator?

template <typename TileType, typename TileCord, typename RndEngType>
class RectRoomGenerator : public RoomGenerator<TileType, TileCord, RndEngType> {
public:
  using RoomGenerator<TileType, TileCord, RndEngType>::RoomMinMax;
  using RoomGenerator<TileType, TileCord, RndEngType>::Ground;
  using RoomGenerator<TileType, TileCord, RndEngType>::Wall;
  using RoomGenerator<TileType, TileCord, RndEngType>::getCtx;

  static const char *Name;

public:
  RectRoomGenerator() = default;
  const char *getName() const override { return Name; }

  Room<TileType, TileCord> generate() override;
};

template <typename T, typename U, typename RE>
const char *RectRoomGenerator<T, U, RE>::Name = "rect_room_generator";

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

template <typename T, typename U, typename RE>
Room<T, U> RectRoomGenerator<T, U, RE>::generate() {
  for (int Attempts = 0; Attempts < 100; Attempts++) {
    const auto RoomSize = randomSize2d<U>(RoomMinMax, getCtx().RndEng);
    auto RoomMap =
        generateMultiRectRoom(*Ground, *Wall, RoomSize, getCtx().RndEng);
    auto RoomDoors = getDoorCandidates(RoomMap, *Ground);
    if (!RoomDoors.empty()) {
      return {std::move(RoomMap), std::move(RoomDoors)};
    }
  }
  throw std::runtime_error("Could not generate new room");
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_RECT_ROOM_GENERATOR_HPP