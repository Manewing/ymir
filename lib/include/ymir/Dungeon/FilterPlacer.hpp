#ifndef YMIR_DUNGEON_FILTER_PLACER_HPP
#define YMIR_DUNGEON_FILTER_PLACER_HPP

#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/RandomBuilder.hpp>

namespace ymir::Dungeon {
template <typename TileType, typename TileCord, typename RandomEngineType>
class FilterPlacer : public RandomBuilder<RandomEngineType> {
public:
  static const char *Type;

public:
  using RandomBuilder<RandomEngineType>::RandomBuilder;

  const char *getType() const override { return Type; }

  void init(BuilderPass &Pass, BuilderContext &C) override;
  void run(BuilderPass &Pass, BuilderContext &C) override;

  bool check4xFilter(const Map<TileType, TileCord> &M,
                     Point2d<TileCord> Pos) const;
  bool check8xFilter(const Map<TileType, TileCord> &M,
                     Point2d<TileCord> Pos) const;

private:
  std::string FilterLayer;
  std::string PlaceLayer;
  std::optional<TileType> FilterTile;
  std::optional<TileType> PlaceTile;
  std::optional<unsigned> Filter8xCountThresMin;
  std::optional<unsigned> Filter8xCountThresMax;
  std::optional<unsigned> Filter4xCountThresMin;
  std::optional<unsigned> Filter4xCountThresMax;
  float PlacePercentage = 80.0;
};

template <typename T, typename U, typename RE>
const char *FilterPlacer<T, U, RE>::Type = "filter_placer";

template <typename T, typename U>
std::vector<ymir::Dungeon::Object<U>>
findPossibleLocations(const Dungeon::Room<T, U> &Room, T Ground, T Wall) {
  std::vector<ymir::Dungeon::Object<U>> Locations;
  Room.M.forEach(
      [&Locations, &Room, Ground, Wall](ymir::Point2d<U> Pos, const T &Tile) {
        if (Tile != Ground || Room.blocksDoor(Pos)) {
          return;
        }
        auto Count = Room.M.getNeighborCount(Pos, Wall);
        if (Count < 3) {
          return;
        }
        Locations.push_back({Pos + Room.Pos, ymir::Dir2d::NONE, false});
      });
  return Locations;
}

template <typename T, typename U, typename RE>
void FilterPlacer<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  FilterLayer = this->template getCfg<std::string>("filter_layer");
  PlaceLayer = this->template getCfg<std::string>("place_layer");
  FilterTile = this->template getCfg<T>("filter_tile");
  PlaceTile = this->template getCfg<T>("place_tile");
  PlacePercentage = this->template getCfg<float>("place_percentage");
  Filter4xCountThresMin =
      this->template getCfgOpt<unsigned>("filter4x_count_thres_min");
  Filter4xCountThresMax =
      this->template getCfgOpt<unsigned>("filter4x_count_thres_max");
  Filter8xCountThresMin =
      this->template getCfgOpt<unsigned>("filter8x_count_thres_min");
  Filter8xCountThresMax =
      this->template getCfgOpt<unsigned>("filter8x_count_thres_max");
}

template <typename T, typename U, typename RE>
bool FilterPlacer<T, U, RE>::check4xFilter(const Map<T, U> &M,
                                           Point2d<U> Pos) const {
  if (!Filter8xCountThresMin || !Filter8xCountThresMax) {
    return false;
  }
  auto Count = M.getNeighborCount(Pos, *FilterTile, FourTileDirections<U>());
  return Count > *Filter4xCountThresMax || Count < *Filter4xCountThresMin;
}

template <typename T, typename U, typename RE>
bool FilterPlacer<T, U, RE>::check8xFilter(const Map<T, U> &M,
                                           Point2d<U> Pos) const {
  if (!Filter4xCountThresMin || !Filter4xCountThresMax) {
    return false;
  }
  auto Count = M.getNeighborCount(Pos, *FilterTile, EightTileDirections<U>());
  return Count > *Filter8xCountThresMax || Count < *Filter8xCountThresMin;
}

template <typename T, typename U, typename RE>
void FilterPlacer<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::run(Pass, C);
  auto &Ctx = C.get<Context<T, U>>();
  auto &FM = Ctx.Map.get(FilterLayer);
  auto &PM = Ctx.Map.get(PlaceLayer);

  std::uniform_real_distribution<float> Uni(0.0, 100);
  FM.forEach([&FM, &PM, this, &Uni](auto Pos, auto &Tile) {
    if (Tile != T()) {
      return;
    }

    if (check4xFilter(FM, Pos) || check8xFilter(FM, Pos) ||
        Uni(this->RndEng) > PlacePercentage) {
      return;
    }
    PM.setTile(Pos, *PlaceTile);
  });
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_FILTER_PLACER_HPP