#ifndef YMIR_CELLULAR_AUTOMATA_HPP
#define YMIR_CELLULAR_AUTOMATA_HPP

#include <optional>
#include <ymir/Map.hpp>

namespace ymir {

namespace celat {

template <typename TileType, typename U>
void generate(Map<TileType, U> &M, TileType Tile, std::size_t NeighborThres,
              std::optional<Rect2d<typename nd<U>::type>> Rect = {}) {
  M.forEach(
      [&M, Tile, NeighborThres](Point2d<U> P, TileType &MapTile) {
        if (M.getNeighborCount(P, Tile) >= NeighborThres) {
          MapTile = Tile;
        }
      },
      Rect);
}

template <typename TileType, typename U>
void replace(Map<TileType, U> &M, TileType TargetTile, TileType ReplaceTile,
             std::size_t NeighborThres,
             std::optional<Rect2d<typename nd<U>::type>> Rect = {}) {
  M.forEach(
      [&M, TargetTile, ReplaceTile, NeighborThres](Point2d<U> P,
                                                   TileType &MapTile) {
        if (MapTile != TargetTile) {
          return;
        }
        if (M.getNeighborCount(P, TargetTile) < NeighborThres) {
          MapTile = ReplaceTile;
        }
      },
      Rect);
}

} // namespace celat

} // namespace ymir

#endif // #ifndef YMIR_CELLULAR_AUTOMATA_HPP