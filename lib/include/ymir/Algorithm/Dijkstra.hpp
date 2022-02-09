#ifndef YMIR_ALGORITHM_DIJKSTRA_HPP
#define YMIR_ALGORITHM_DIJKSTRA_HPP

#include <queue>
#include <tuple>
#include <ymir/Map.hpp>
#include <ymir/Types.hpp>
#include <limits>

namespace ymir::Algorithm {

template <typename TileCord>
auto getDijkstraQueue(const std::vector<ymir::Point2d<TileCord>> &Starts) {
  using PosDist = std::tuple<int, TileCord, TileCord>;
  std::priority_queue<PosDist, std::vector<PosDist>, std::greater<PosDist>> Q;
  for (const auto &Start : Starts) {
    Q.push({0, Start.X, Start.Y});
  }
  return Q;
}

template <typename TileCord, typename UnaryPred,
          typename DirectionProvider = FourTileDirections<TileCord>>
ymir::Map<int, TileCord> getDijkstraMap(
    ymir::Size2d<TileCord> MapSize,
    const std::vector<ymir::Point2d<TileCord>> &Starts, UnaryPred IsBlocked,
    [[maybe_unused]] DirectionProvider DirProv = DirectionProvider()) {
  ymir::Map<int, TileCord> DM(MapSize);

  // Mark the entire map as unvisited
  DM.fill(-1);

  auto Queue = getDijkstraQueue(Starts);

  while (!Queue.empty()) {
    auto [Dist, PosX, PosY] = Queue.top();
    ymir::Point2d<TileCord> Pos(PosX, PosY);
    Queue.pop();
    DM.setTile(Pos, Dist);

    // Queue everything that is not blocked and not yet visited in reach of the
    // current position
    int NextDist = Dist + 1;
    auto QueuePos = [&Queue, &IsBlocked, NextDist](auto Pos, auto &Tile) {
      if (IsBlocked(Pos) || Tile != -1) {
        return true;
      }
      Tile = NextDist;
      Queue.push({NextDist, Pos.X, Pos.Y});
      return true;
    };
    DirectionProvider::forEach(DM, Pos, QueuePos);
  }
  return DM;
}

template <typename TileCord, typename UnaryPred,
          typename DirectionProvider = FourTileDirections<TileCord>>
ymir::Map<int, TileCord>
getDijkstraMap(ymir::Size2d<TileCord> MapSize, ymir::Point2d<TileCord> Start,
               UnaryPred IsBlocked,
               DirectionProvider DirProv = DirectionProvider()) {
  return getDijkstraMap<TileCord, UnaryPred, DirectionProvider>(
      MapSize, std::vector{Start}, IsBlocked, DirProv);
}

// Returns a path from a dijkstra map, starting at End finds path towards Start
// for which dijkstra map was created.
template <typename TileCord,
          typename DirectionProvider = FourTileDirections<TileCord>>
std::vector<ymir::Point2d<TileCord>> getPathFromDijkstraMap(
    const ymir::Map<int, TileCord> &DM, ymir::Point2d<TileCord> End,
    DirectionProvider DirProv = DirectionProvider(), unsigned MaxLength = -1) {
  int Dist = std::numeric_limits<int>::max();
  std::vector<ymir::Point2d<TileCord>> Path = {};
  ymir::Point2d<TileCord> Pos = End;
  while (Dist != 0 && Path.size() < MaxLength) {

    ymir::Point2d<TileCord> NextPos = Pos;
    auto FindMinDist = [&NextPos, &Dist](auto Pos, auto &Tile) {
      if (Tile < Dist && Tile != -1) {
        Dist = Tile;
        NextPos = Pos;
      }
      return true;
    };
    DirProv.forEach(DM, Pos, FindMinDist);
    if (NextPos == Pos) {
      return {};
    }
    Pos = NextPos;
    Path.push_back(Pos);
  }

  return Path;
}

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_DIJKSTRA_HPP