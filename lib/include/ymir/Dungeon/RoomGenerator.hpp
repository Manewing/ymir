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
  virtual Room<TileType, TileCord> generate() = 0;

protected:
  CtxType &getCtx() { return BuilderBase::getCtx<CtxType>(); }

protected:
  std::optional<TileType> Ground;
  std::optional<TileType> Wall;
};

template <typename T, typename U, typename RE>
const char *RoomGenerator<T, U, RE>::Name = "room_generator";

template <typename T, typename U, typename RE>
void RoomGenerator<T, U, RE>::init(BuilderPass &Pass, BuilderContext &Ctx) {
  BuilderBase::init(Pass, Ctx);
  Ground = getPass().template getConfigValue<T>("ground");
  Wall = getPass().template getConfigValue<T>("wall");
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_ROOM_GENERATOR_HPP
