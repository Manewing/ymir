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
  virtual Room<TileType, TileCord> generate() = 0;
  bool testFunc() { return false; }

protected:
  CtxType &getCtx() { return BuilderBase::getCtx<CtxType>(); }
};

template <typename T, typename U, typename RE>
const char *RoomGenerator<T, U, RE>::Name = "room_generator";

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_ROOM_GENERATOR_HPP
