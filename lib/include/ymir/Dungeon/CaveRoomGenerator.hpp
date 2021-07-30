#ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP

#include <ymir/CallularAutomata.hpp>
#include <ymir/Dungeon/RoomGenerator.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RndEngType>
class CaveRoomGenerator : public RoomGenerator<TileType, TileCord, RndEngType> {
public:
  using RoomGenerator<TileType, TileCord, RndEngType>::Ground;
  using RoomGenerator<TileType, TileCord, RndEngType>::RoomMinMax;
  using RoomGenerator<TileType, TileCord, RndEngType>::Wall;
  using RoomGenerator<TileType, TileCord, RndEngType>::getCtx;

  static const char *Name;

public:
  CaveRoomGenerator() = default;
  const char *getName() const override { return Name; }

  Room<TileType, TileCord> generate() override;
};

template <typename T, typename U, typename RE>
const char *CaveRoomGenerator<T, U, RE>::Name = "cave_room_generator";

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
Room<T, U> CaveRoomGenerator<T, U, RE>::generate() {
  for (int Attempts = 0; Attempts < 100; Attempts++) {
    const auto RoomSize = randomSize2d<U>(RoomMinMax, getCtx().RndEng);
    auto RoomMap = generateCaveRoom(*Ground, *Wall, RoomSize, getCtx().RndEng);
    auto RoomDoors = getDoorCandidates(RoomMap, *Ground);
    if (!RoomDoors.empty()) {
      return {std::move(RoomMap), std::move(RoomDoors)};
    }
  }
  throw std::runtime_error("Could not generate new room");
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP