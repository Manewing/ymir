#ifndef YMIR_ALGORITHM_DIJKSTRA_HPP
#define YMIR_ALGORITHM_DIJKSTRA_HPP

#include <limits>
#include <queue>
#include <tuple>
#include <ymir/Map.hpp>
#include <ymir/Types.hpp>

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
std::vector<ymir::Point2d<TileCord>>
getPathFromDijkstraMap(const ymir::Map<int, TileCord> &DM,
                       const ymir::Point2d<TileCord> &Start,
                       const ymir::Point2d<TileCord> &End,
                       DirectionProvider DirProv = DirectionProvider(),
                       unsigned MaxLength = -1, bool AllowCloserEqual = false) {
  int Dist = std::numeric_limits<int>::max();
  std::vector<ymir::Point2d<TileCord>> Path = {};
  ymir::Point2d<TileCord> PathPos = End;

  while (Dist != 0 && Path.size() < MaxLength) {
    ymir::Point2d<TileCord> NextPos = PathPos;
    const auto FindMinDist = [&NextPos, &Dist, &Start,
                              AllowCloserEqual](auto Pos, auto &Tile) {
      if ((Tile < Dist ||
           (Tile <= Dist && Point2d<TileCord>::isCloser(Pos, NextPos, Start,
                                                        AllowCloserEqual))) &&
          Tile != -1) {
        Dist = Tile;
        NextPos = Pos;
      }
      return true;
    };
    DirProv.forEach(DM, PathPos, FindMinDist);
    if (NextPos == PathPos) {
      return {};
    }
    PathPos = NextPos;
    Path.push_back(PathPos);
  }

  return Path;
}

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_DIJKSTRA_HPP