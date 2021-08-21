#ifndef YMIR_DUNGEON_CELALT_MAP_FILLER_HPP
#define YMIR_DUNGEON_CELALT_MAP_FILLER_HPP

#include <ymir/Dungeon/BuilderBase.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RandEngType>
class CelAltMapFiller : public BuilderBase {
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
  std::optional<TileType> ClearTile;
  std::optional<Rect2d<TileCord>> Rect;
  unsigned ReplaceThres = 2;
  unsigned Iterations = 6;
  unsigned SmoothThres = 7;
  unsigned KillThres = 8;
  float SpawnChance = 0.5f;
};

template <typename T, typename U, typename RE>
const char *CelAltMapFiller<T, U, RE>::Type = "celalt_map_filler";

template <typename T, typename U, typename RE>
void CelAltMapFiller<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  Layer = getCfg<std::string>("layer");
  Tile = getCfg<T>("tile");
  ClearTile = getCfgOr<T>("clear_tile", T());
  Rect = getCfgOpt<Rect2d<U>>("rect");
  SpawnChance = getCfgOr<float>("spawn_chance", SpawnChance);
  ReplaceThres = getCfgOr<unsigned>("replace_thres", ReplaceThres);
  Iterations = getCfgOr<unsigned>("iterations", Iterations);
  SmoothThres = getCfgOr<unsigned>("smooth_thres", SmoothThres);
  KillThres = getCfgOr<unsigned>("kill_thres", KillThres);
}

template <typename T, typename U, typename RE>
void CelAltMapFiller<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::run(Pass, C);
  auto &Ctx = C.get<Context<T, U, RE>>();

  // FIXME we could use a buffer map here and by that avoid killing previously
  // generated patterns

  // Get the map layer
  auto &M = Ctx.Map.get(Layer);

  // Clear map
  M.fillRect(*ClearTile);

  // Generate initial noise
  Rect2d<U> R = M.rect();
  if (Rect.has_value()) {
    R = *Rect;
  }
  ymir::fillRectRandom(M, *Tile, SpawnChance, Ctx.RndEng, R);

  // Run replacement
  for (std::size_t Idx = 0; Idx < Iterations; Idx++) {
    ymir::celat::replace(M, *Tile, *ClearTile, ReplaceThres);
  }

  // Smooth generation
  ymir::celat::generate(M, *Tile, SmoothThres);

  // Kill isolated tiles
  ymir::celat::generate(M, *ClearTile, KillThres);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CELALT_MAP_FILLER_HPP