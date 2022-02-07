#ifndef YMIR_DUNGEON_START_END_PLACER_HPP
#define YMIR_DUNGEON_START_END_PLACER_HPP

#include <ymir/Algorithm/Dijkstra.hpp>
#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/RandomBuilder.hpp>
#include <ymir/Terminal.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RandomEngineType>
class StartEndPlacer : public RandomBuilder<RandomEngineType> {
public:
  static const char *Type;

public:
  using RandomBuilder<RandomEngineType>::RandomBuilder;

  const char *getType() const override { return Type; }

  void init(BuilderPass &Pass, BuilderContext &C) override;

  Point2d<TileCord> findFreeTileForStart();
  std::vector<Point2d<TileCord>>
  findFreeTilesForEnd(Point2d<TileCord> StartPos);

  void run(BuilderPass &Pass, BuilderContext &C) override;

private:
  std::string Layer;
  std::optional<TileType> StartTile;
  std::optional<TileType> EndTile;
  unsigned NumEnds = 1;
  float DistanceThres = 0.6f;
};

template <typename T, typename U, typename RE>
const char *StartEndPlacer<T, U, RE>::Type = "start_end_placer";

template <typename T, typename U, typename RE>
void StartEndPlacer<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  Layer = this->template getCfg<std::string>("layer");
  StartTile = this->template getCfg<T>("start_tile");
  EndTile = this->template getCfg<T>("end_tile");
  NumEnds = this->template getCfgOr<unsigned>("num_ends", NumEnds);
  DistanceThres =
      this->template getCfgOr<float>("distance_thres", DistanceThres);
}

template <typename T, typename U, typename RE>
Point2d<U> StartEndPlacer<T, U, RE>::findFreeTileForStart() {
  auto &Ctx = this->template getCtx<Context<T, U>>();
  auto &Map = Ctx.Map.get(Layer);

  std::vector<ymir::Point2d<U>> FreeTiles;
  Map.forEach([&Ctx, &FreeTiles](auto Pos, auto &Tile) {
    if (Tile != T() || !Ctx.isInRoom(Pos)) {
      return;
    }
    FreeTiles.push_back(Pos);
  });
  if (FreeTiles.empty()) {
    throw std::runtime_error("Could not find free tile to place start");
  }
  auto It = randomIterator(FreeTiles.begin(), FreeTiles.end(), this->RndEng);
  return *It;
}

template <typename T, typename U, typename RE>
std::vector<Point2d<U>>
StartEndPlacer<T, U, RE>::findFreeTilesForEnd(Point2d<U> StartPos) {
  auto &Ctx = this->template getCtx<Context<T, U>>();
  auto &Map = Ctx.Map.get(Layer);

  auto DM =
      Algorithm::getDijkstraMap(Map.getSize(), StartPos, [&Map](auto Pos) {
        return Map.getTile(Pos) != T();
      });
  int MaxDistance = *std::max_element(DM.begin(), DM.end());
  int MinEndDistance = MaxDistance * DistanceThres;

  std::vector<ymir::Point2d<U>> PossibleEnds;
  Map.forEach([&DM, MinEndDistance, &PossibleEnds, &Ctx](auto Pos, auto &Tile) {
    if (Tile != T() || DM.getTile(Pos) < MinEndDistance ||
        !Ctx.isInRoom(Pos)) {
      return;
    }
    PossibleEnds.push_back(Pos);
  });
  if (PossibleEnds.size() < NumEnds) {
    throw std::runtime_error("Could not find free tile to place end");
  }

  return PossibleEnds;
}

template <typename T, typename U, typename RE>
void StartEndPlacer<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::run(Pass, C);
  auto &Ctx = C.get<Context<T, U>>();
  auto &Map = Ctx.Map.get(Layer);

  auto StartPos = findFreeTileForStart();
  auto PossibleEnds = findFreeTilesForEnd(StartPos);

  for (unsigned Idx = 0; Idx < NumEnds; Idx++) {
    auto It =
        randomIterator(PossibleEnds.begin(), PossibleEnds.end(), this->RndEng);
    Map.setTile(*It, *EndTile);
    PossibleEnds.erase(It);
  }

  Map.setTile(StartPos, *StartTile);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_START_END_PLACER_HPP