#ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP

#include <ymir/CallularAutomata.hpp>
#include <ymir/Dungeon/RoomGenerator.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RndEngType>
class CaveRoomGenerator : public RoomGenerator<TileType, TileCord, RndEngType> {
public:
  using RoomGeneratorType = RoomGenerator<TileType, TileCord, RndEngType>;

public:
  static const char *Type;

public:
  using RoomGeneratorType::RoomGenerator;
  const char *getType() const override { return Type; }

  Map<TileType, TileCord> generateRoomMap(Size2d<TileCord> Size) override;
};

template <typename T, typename U, typename RE>
const char *CaveRoomGenerator<T, U, RE>::Type = "cave_room_generator";

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

template <typename T, typename U, typename RE>
Map<T, U> CaveRoomGenerator<T, U, RE>::generateRoomMap(Size2d<U> Size) {
  return generateCaveRoom(T(), *this->Wall, Size, this->RndEng);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP