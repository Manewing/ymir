#ifndef YMIR_DUNGEON_RECT_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_RECT_ROOM_GENERATOR_HPP

#include <ymir/Dungeon/RoomGenerator.hpp>

namespace ymir::Dungeon {

// TODO rename MultiRectRoomGenerator and add (single) RectRoomGenerator?

template <typename TileType, typename TileCord, typename RndEngType>
class RectRoomGenerator : public RoomGenerator<TileType, TileCord, RndEngType> {
public:
  using RoomGeneratorType = RoomGenerator<TileType, TileCord, RndEngType>;

public:
  static const char *Type;

public:
  using RoomGeneratorType::RoomGenerator;
  const char *getType() const override { return Type; }

  Map<TileType, TileCord> generateRoomMap(Size2d<TileCord> SIze) override;
};

template <typename T, typename U, typename RE>
const char *RectRoomGenerator<T, U, RE>::Type = "rect_room_generator";

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
Map<T, U> RectRoomGenerator<T, U, RE>::generateRoomMap(Size2d<U> Size) {
  return generateMultiRectRoom(T(), *this->Wall, Size, this->getCtx().RndEng);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_RECT_ROOM_GENERATOR_HPP