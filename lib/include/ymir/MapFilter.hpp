#ifndef YMIR_MAP_FILTER_HPP
#define YMIR_MAP_FILTER_HPP

#include <ymir/Map.hpp>

namespace ymir {

template <typename T, typename U>
U verticalEdgeFilter(const ymir::Map<T, U> &M, ymir::Point2d<U> P, T Tile) {
  U Value = 0;
  Value += M.getTileOr({P.X - 1, P.Y + 1}) != Tile;
  Value += M.getTileOr({P.X + 0, P.Y + 1}) != Tile;
  Value += M.getTileOr({P.X + 1, P.Y + 1}) != Tile;
  Value -= M.getTileOr({P.X - 1, P.Y - 1}) != Tile;
  Value -= M.getTileOr({P.X + 0, P.Y - 1}) != Tile;
  Value -= M.getTileOr({P.X + 1, P.Y - 1}) != Tile;
  return Value;
}

template <typename T, typename U>
U horizontalEdgeFilter(const ymir::Map<T, U> &M, ymir::Point2d<U> P, T Tile) {
  U Value = 0;
  Value += M.getTileOr({P.X + 1, P.Y - 1}) != Tile;
  Value += M.getTileOr({P.X + 1, P.Y + 0}) != Tile;
  Value += M.getTileOr({P.X + 1, P.Y + 1}) != Tile;
  Value -= M.getTileOr({P.X - 1, P.Y - 1}) != Tile;
  Value -= M.getTileOr({P.X - 1, P.Y + 0}) != Tile;
  Value -= M.getTileOr({P.X - 1, P.Y + 1}) != Tile;
  return Value;
}

} // namespace ymir

#endif // #ifndef YMIR_MAP_FILTER_HPP