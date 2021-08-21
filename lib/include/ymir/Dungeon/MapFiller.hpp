#ifndef YMIR_DUNGEON_MAP_FILLER_HPP
#define YMIR_DUNGEON_MAP_FILLER_HPP

#include <ymir/Dungeon/BuilderBase.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord>
class MapFiller : public BuilderBase {
public:
  static const char *Type;

public:
  using BuilderBase::BuilderBase;

  const char *getType() const override { return Type; }
  void init(BuilderPass &Pass, BuilderContext &C) override;
  void run(BuilderPass &Pass, BuilderContext &C) override;

private:
  std::string Layer;
  std::optional<TileType> Tile;
};

template <typename T, typename U>
const char *MapFiller<T, U>::Type = "map_filler";

template <typename T, typename U>
void MapFiller<T, U>::init(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  Layer = getCfg<std::string>("layer");
  Tile = getCfg<T>("tile");
}

template <typename T, typename U>
void MapFiller<T, U>::run(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::run(Pass, C);
  auto &Ctx = C.get<Context<T, U>>();
  Ctx.Map.get(Layer).fillRect(*Tile);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_MAP_FILLER_HPP