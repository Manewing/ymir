#ifndef YMIR_ALGORITHM_DIJKSTRA_HPP
#define YMIR_ALGORITHM_DIJKSTRA_HPP

#include <queue>
#include <tuple>
#include <ymir/Map.hpp>

namespace ymir::Algorithm {

template <typename TileCord, typename Derived> struct TileDirections {
  template <typename MapType, typename BinaryFunc>
  static void forEach(MapType &Map, ymir::Point2d<TileCord> Start,
                      BinaryFunc Func) {
    for (const auto &Direction : Derived::get()) {
      auto Pos = Start + Direction;
      if (!Map.contains(Pos)) {
        continue;
      }
      auto &Tile = Map.getTile(Pos);
      Func(Pos, Tile);
    }
  }
};

template <typename TileCord>
struct FourTileDirections
    : public TileDirections<TileCord, FourTileDirections<TileCord>> {
  static const auto &get() {
    static const std::array Directions = {
        ymir::Point2d<TileCord>{-1, 0},
        ymir::Point2d<TileCord>{1, 0},
        ymir::Point2d<TileCord>{0, -1},
        ymir::Point2d<TileCord>{0, 1},
    };
    return Directions;
  }
};

template <typename TileCord>
struct EightTileDirections
    : public TileDirections<TileCord, EightTileDirections<TileCord>> {
  static const auto &get() {
    static const std::array Directions = {
        ymir::Point2d<TileCord>{-1, 0}, ymir::Point2d<TileCord>{1, 0},
        ymir::Point2d<TileCord>{0, -1}, ymir::Point2d<TileCord>{0, 1},
        ymir::Point2d<TileCord>{-1, 1}, ymir::Point2d<TileCord>{-1, -1},
        ymir::Point2d<TileCord>{1, 1},  ymir::Point2d<TileCord>{1, -1},
    };
    return Directions;
  }
};

template <typename TileCord>
auto getDijkstraQueue(const std::vector<ymir::Point2d<TileCord>> &Starts) {
  using PosDist = std::tuple<unsigned, TileCord, TileCord>;
  std::priority_queue<PosDist, std::vector<PosDist>, std::greater<PosDist>> Q;
  for (const auto &Start : Starts) {
    Q.push({0, Start.X, Start.Y});
  }
  return Q;
}

template <typename TileCord, typename UnaryPred,
          typename DirectionProvider = FourTileDirections<TileCord>>
ymir::Map<unsigned, TileCord>
getDijkstraMap(ymir::Size2d<TileCord> MapSize,
               const std::vector<ymir::Point2d<TileCord>> &Starts,
               UnaryPred IsBlocked) {
  ymir::Map<unsigned, TileCord> DM(MapSize);
  DM.fill(-1);

  auto Queue = getDijkstraQueue(Starts);

  while (!Queue.empty()) {
    auto [Dist, PosX, PosY] = Queue.top();
    ymir::Point2d<TileCord> Pos(PosX, PosY);
    Queue.pop();
    DM.setTile(Pos, Dist);

    unsigned NextDist = Dist + 1;
    auto QueuePos = [&Queue, &IsBlocked, NextDist](auto Pos, auto &Tile) {
      if (IsBlocked(Pos) || Tile != unsigned(-1)) {
        return;
      }
      Tile = NextDist;
      Queue.push({NextDist, Pos.X, Pos.Y});
    };
    DirectionProvider::forEach(DM, Pos, QueuePos);
  }
  return DM;
}

template <typename TileCord, typename UnaryPred,
          typename DirectionProvider = FourTileDirections<TileCord>>
ymir::Map<unsigned, TileCord> getDijkstraMap(ymir::Size2d<TileCord> MapSize,
                                             ymir::Point2d<TileCord> Start,
                                             UnaryPred IsBlocked) {
  return getDijkstraMap<TileCord, UnaryPred, DirectionProvider>(
      MapSize, std::vector{Start}, IsBlocked);
}

template <typename TileCord,
          typename DirectionProvider = FourTileDirections<TileCord>>
std::vector<ymir::Point2d<TileCord>>
getPathFromDijkstraMap(const ymir::Map<unsigned, TileCord> &DM,
                       ymir::Point2d<TileCord> End) {
  unsigned Dist = DM.getTile(End);
  if (Dist == unsigned(-1)) {
    return {};
  }

  std::vector<ymir::Point2d<TileCord>> Path = {End};
  ymir::Point2d<TileCord> Pos = End;
  while (Dist != 0) {

    ymir::Point2d<TileCord> NextPos;

    auto FindMinDist = [&NextPos, &Dist](auto Pos, auto &Tile) {
      if (Tile < Dist) {
        Dist = Tile;
        NextPos = Pos;
      }
    };
    DirectionProvider::forEach(DM, Pos, FindMinDist);
    Pos = NextPos;
    Path.push_back(Pos);
  }

  return Path;
}

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_DIJKSTRA_HPP