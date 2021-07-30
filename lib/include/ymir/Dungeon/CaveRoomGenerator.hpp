#ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP

#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/Room.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RandEng>
class CaveRoomGenerator : public BuilderBase {
public:
  constexpr char *Name = "cave_room_generator";

public:
  CaveRoomGenerator() = default;

  const char *getName() const override { return Name; }

  void init(BuilderPass &Pass, BuilderContext &C) override {
    CtxPtr = C.get_or_null<Context<TileType, TileCord, RandEng>();
    assert(CtxPtr); // TODO
  }

  void run(BuilderPass &Pass, BuilderContext &C) override {
    // Nothing todo?
    (void)Pass;
    (void)C;
    /// auto &Ctx = C.get<Context<TileType, TileCord, RandEng>>();
  }

  Room<TileType, TileCord> generate() {
    const Rect2d<TileCord> RoomMinMax = {{5, 5}, {10, 10}};
    const auto RoomSize = randomSize2d<TileCord>(RoomMinMax, CtxPtr->RndEng);
    auto RoomMap = generateCaveRoom(CtxPtr->Ground, CtxPtr->Wall, RoomSize, CtxPtr->RndEng);
    auto RoomDoors = getDoorCandidates(RooMap, CtxPtr->Ground);
  }

private:
  Context<TileType, TileCord, RandEng> *CtxPtr = nullptr;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CAVE_ROOM_GENERATOR_HPP