#ifndef YMIR_DUNGEON_CELALT_MAP_FILLER_HPP
#define YMIR_DUNGEON_CELALT_MAP_FILLER_HPP

#include <ymir/Dungeon/RandomBuilder.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RandEngType>
class CelAltMapFiller : public RandomBuilder<RandEngType> {
public:
  static const char *Type;

public:
  using RandomBuilder<RandEngType>::RandomBuilder;

  const char *getType() const override { return Type; }
  void init(BuilderPass &Pass, BuilderContext &C) override;
  void run(BuilderPass &Pass, BuilderContext &C) override;

private:
  std::string Layer;
  TileType Tile;
  TileType ClearTile;
  std::optional<Rect2d<TileCord>> Rect;
  unsigned ReplaceThres = 2;
  unsigned Iterations = 6;
  unsigned SmoothThres = 7;
  unsigned KillThres = 8;
  float SpawnChance = 0.5f;
  bool HasBorder = true;
};

template <typename T, typename U, typename RE>
const char *CelAltMapFiller<T, U, RE>::Type = "celalt_map_filler";

template <typename T, typename U, typename RE>
void CelAltMapFiller<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  RandomBuilder<RE>::init(Pass, C);
  Layer = this->template getCfg<std::string>("layer");
  Tile = this->template getCfgOr<T>("tile", T());
  ClearTile = this->template getCfgOr<T>("clear_tile", T());
  Rect = this->template getCfgOpt<Rect2d<U>>("rect");
  SpawnChance = this->template getCfgOr<float>("spawn_chance", SpawnChance);
  ReplaceThres =
      this->template getCfgOr<unsigned>("replace_thres", ReplaceThres);
  Iterations = this->template getCfgOr<unsigned>("iterations", Iterations);
  SmoothThres = this->template getCfgOr<unsigned>("smooth_thres", SmoothThres);
  KillThres = this->template getCfgOr<unsigned>("kill_thres", KillThres);
  HasBorder = this->template getCfgOr<bool>("has_border", HasBorder);
}

template <typename T, typename U, typename RE>
void CelAltMapFiller<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  RandomBuilder<RE>::run(Pass, C);
  auto &Ctx = C.get<Context<T, U>>();

  // FIXME we could use a buffer map here and by that avoid killing previously
  // generated patterns

  // Get the map layer
  auto &M = Ctx.Map.get(Layer);

  // Generate initial noise
  Rect2d<U> R = M.rect();
  if (Rect.has_value()) {
    R = *Rect;
  }
  ymir::fillRectRandom(M,Tile, SpawnChance, this->RndEng, R);

  // Run replacement
  for (std::size_t Idx = 0; Idx < Iterations; Idx++) {
    ymir::celat::replace(M,Tile, ClearTile, ReplaceThres);
  }

  // Smooth generation
  ymir::celat::generate(M,Tile, SmoothThres);

  // Kill isolated tiles
  ymir::celat::generate(M,ClearTile, KillThres);

}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CELALT_MAP_FILLER_HPP
