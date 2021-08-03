#ifndef YMIR_MAP_HPP
#define YMIR_MAP_HPP

#include <array>
#include <cassert>
#include <iostream>
#include <optional>
#include <vector>
#include <ymir/Types.hpp>

namespace ymir {

template <typename T, typename U = int> class Map {
public:
  using TileType = T;
  using TileCord = U;
  using TilePos = Point2d<TileCord>;

  template <typename TX, typename UX>
  friend bool operator==(const Map<TX, UX> &Lhs, const Map<TX, UX> &Rhs);

public:
  // TODO allow passing tile for default value when resizing
  explicit Map(Size2d<TileCord> Size) : Size(Size) {
    Data.resize(Size.W * Size.H);
  }

  Map(TileCord Width, TileCord Height) : Map(Size2d<TileCord>{Width, Height}) {}

  inline Size2d<TileCord> getSize() const { return Size; }

  inline void setTile(TilePos P, TileType Tile) { getTile(P) = Tile; }

  inline TileType &getTile(TilePos P) { return Data.at(P.X + P.Y * Size.W); }

  inline const TileType &getTile(TilePos P) const {
    return Data.at(P.X + P.Y * Size.W);
  }

  inline bool isTile(TilePos P, TileType Tile) const {
    if (static_cast<unsigned>(P.X + P.Y * Size.W) >= Data.size()) {
      return false;
    }
    return getTile(P) == Tile;
  }

  std::size_t getNeighborCount(TilePos P, TileType Tile) const {
    std::size_t Count = 0;
    Count += isTile({P.X + 1, P.Y + 1}, Tile);
    Count += isTile({P.X + 1, P.Y - 1}, Tile);
    Count += isTile({P.X + 1, P.Y}, Tile);
    Count += isTile({P.X - 1, P.Y + 1}, Tile);
    Count += isTile({P.X - 1, P.Y - 1}, Tile);
    Count += isTile({P.X - 1, P.Y}, Tile);
    Count += isTile({P.X, P.Y + 1}, Tile);
    Count += isTile({P.X, P.Y - 1}, Tile);
    return Count;
  }

  Rect2d<TileCord> rect() const { return Rect2d<TileCord>{{0, 0}, Size}; }

  bool contains(Point2d<TileCord> P) const { return rect().contains(P); }

  Rect2d<TileCord>
  getContained(std::optional<Rect2d<TileCord>> Rect = {}) const {
    if (Rect) {
      return rect() & *Rect;
    }
    return rect();
  }

  template <typename UnaryFunction>
  void forEachElem(UnaryFunction Func,
                   std::optional<Rect2d<TileCord>> Rect = {}) const {
    auto R = getContained(Rect);
    for (auto PY = R.Pos.Y; PY < R.Pos.Y + R.Size.H && PY < Size.H; PY++) {
      for (auto PX = R.Pos.X; PX < R.Pos.X + R.Size.W && PX < Size.W; PX++) {
        Func(getTile({PX, PY}));
      }
    }
  }

  template <typename UnaryFunction>
  void forEachElem(UnaryFunction Func,
                   std::optional<Rect2d<TileCord>> Rect = {}) {
    auto R = getContained(Rect);
    for (auto PY = R.Pos.Y; PY < R.Pos.Y + R.Size.H && PY < Size.H; PY++) {
      for (auto PX = R.Pos.X; PX < R.Pos.X + R.Size.W && PX < Size.W; PX++) {
        Func(getTile({PX, PY}));
      }
    }
  }

  template <typename BinaryFunction>
  void forEach(BinaryFunction Func,
               std::optional<Rect2d<TileCord>> Rect = {}) const {
    auto R = getContained(Rect);
    for (auto PY = R.Pos.Y; PY < R.Pos.Y + R.Size.H && PY < Size.H; PY++) {
      for (auto PX = R.Pos.X; PX < R.Pos.X + R.Size.W && PX < Size.W; PX++) {
        Func({PX, PY}, getTile({PX, PY}));
      }
    }
  }

  template <typename BinaryFunction>
  void forEach(BinaryFunction Func, std::optional<Rect2d<TileCord>> Rect = {}) {
    auto R = getContained(Rect);
    for (auto PY = R.Pos.Y; PY < R.Pos.Y + R.Size.H && PY < Size.H; PY++) {
      for (auto PX = R.Pos.X; PX < R.Pos.X + R.Size.W && PX < Size.W; PX++) {
        Func({PX, PY}, getTile({PX, PY}));
      }
    }
  }

  void fillRect(TileType Tile, std::optional<Rect2d<TileCord>> Rect = {}) {
    forEachElem([&Tile](TileType &TL) { TL = Tile; }, Rect);
  }

  void merge(const Map &Other, Point2d<TileCord> Pos = {0, 0}) {
    auto R = getContained(Rect2d<TileCord>{Pos, Other.Size});
    for (auto PY = R.Pos.Y; PY < R.Pos.Y + R.Size.H && PY < Size.H; PY++) {
      for (auto PX = R.Pos.X; PX < R.Pos.X + R.Size.W && PX < Size.W; PX++) {
        getTile({PX, PY}) = Other.getTile({PX - Pos.X, PY - Pos.Y});
      }
    }
  }

private:
  Size2d<TileCord> Size;
  std::vector<TileType> Data;
};

template <typename T, typename U>
std::ostream &operator<<(std::ostream &Out, const Map<T, U> &M) {
  for (auto PY = 0; PY < M.getSize().H; PY++) {
    for (auto PX = 0; PX < M.getSize().W; PX++) {
      Out << M.getTile({PX, PY});
    }
    Out << '\n';
  }
  return Out;
}

template <typename T, typename U>
inline bool operator==(const Map<T, U> &Lhs, const Map<T, U> &Rhs) {
  return Lhs.Data == Rhs.Data;
}

} // namespace ymir

#endif // #ifndef YMIR_MAP_H