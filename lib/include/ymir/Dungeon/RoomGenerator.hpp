#ifndef YMIR_DUNGEON_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_ROOM_GENERATOR_HPP

#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/Room.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RndEngType>
class RoomGenerator : public BuilderBase {
public:
  static const char *Name;

public:
  using CtxType = Context<TileType, TileCord, RndEngType>;

public:
  RoomGenerator() = default;

  void init(BuilderPass &Pass, BuilderContext &C) override;
  virtual Room<TileType, TileCord> generate();

  virtual Map<TileType, TileCord> generateRoomMap(Size2d<TileCord> Size) = 0;

protected:
  CtxType &getCtx() { return BuilderBase::getCtx<CtxType>(); }

protected:
  std::optional<TileType> Ground;
  std::optional<TileType> Wall;
  Rect2d<TileCord> RoomMinMax;
};

template <typename T, typename U, typename RE>
const char *RoomGenerator<T, U, RE>::Name = "room_generator";

template <typename T, typename U, typename RE>
void RoomGenerator<T, U, RE>::init(BuilderPass &Pass, BuilderContext &Ctx) {
  BuilderBase::init(Pass, Ctx);
  Ground = getPass().cfg(). template get<T>("ground");
  Wall = getPass().cfg(). template get<T>("wall");
  RoomMinMax = getPass().cfg(). template get<Rect2d<U>>(
      "room_generator/room_size_min_max");
}

template <typename T, typename U, typename RE>
Room<T, U> RoomGenerator<T, U, RE>::generate() {
  // FIXME get rid of this or at least make configurable
  for (int Attempts = 0; Attempts < 100; Attempts++) {
    const auto RoomSize = randomSize2d<U>(this->RoomMinMax, getCtx().RndEng);
    auto RoomMap = generateRoomMap(RoomSize);
    auto RoomDoors = getDoorCandidates(RoomMap, *this->Ground);
    if (!RoomDoors.empty()) {
      return {std::move(RoomMap), std::move(RoomDoors)};
    }
  }
  throw std::runtime_error("Could not generate new room");
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_ROOM_GENERATOR_HPP
