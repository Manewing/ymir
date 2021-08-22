#ifndef YMIR_MAP_HPP
#define YMIR_MAP_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <optional>
#include <iterator>
#include <vector>
#include <ymir/Types.hpp>

namespace ymir {

template <typename T, typename U = int> class Map {
public:
  using TileType = T;
  using TileCord = U;
  using TilePos = Point2d<TileCord>;
  using DataType = std::vector<TileType>;

  using iterator = typename DataType::iterator;
  using const_iterator = typename DataType::const_iterator;

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

  void setTiles(const std::vector<ymir::Point2d<TileCord>> &TilePos,
                TileType Tile) {
    for (auto const &Pos : TilePos) {
      setTile(Pos, Tile);
    }
  }

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
        Func(ymir::Point2d<TileCord>{PX, PY}, getTile({PX, PY}));
      }
    }
  }

  template <typename BinaryFunction>
  void forEach(BinaryFunction Func, std::optional<Rect2d<TileCord>> Rect = {}) {
    auto R = getContained(Rect);
    for (auto PY = R.Pos.Y; PY < R.Pos.Y + R.Size.H && PY < Size.H; PY++) {
      for (auto PX = R.Pos.X; PX < R.Pos.X + R.Size.W && PX < Size.W; PX++) {
        Func(ymir::Point2d<TileCord>{PX, PY}, getTile({PX, PY}));
      }
    }
  }

  void fill(TileType Tile) { std::fill(Data.begin(), Data.end(), Tile); }

  void fillRect(TileType Tile, std::optional<Rect2d<TileCord>> Rect = {}) {
    forEachElem([&Tile](TileType &TL) { TL = Tile; }, Rect);
  }

  void replaceTile(TileType Target, TileType Replacement) {
    std::replace(Data.begin(), Data.end(), Target, Replacement);
  }

  std::vector<ymir::Point2d<TileCord>> findTiles(TileType Target) const {
    std::vector<ymir::Point2d<TileCord>> Result;
    forEach([&Target, &Result](auto Pos, const auto &Tile) {
      if (Target == Tile) {
        Result.push_back(Pos);
      }
    });
    return Result;
  }

  iterator begin() { return Data.begin(); }
  iterator end() { return Data.end(); }
  const_iterator begin() const { return Data.begin(); }
  const_iterator end() const { return Data.end(); }

  ymir::Point2d<TileCord> toPos(iterator It) {
    auto Dist = static_cast<TileCord>(std::distance(Data.begin(), It));
    return {Dist % Size.W, Dist / Size.W};
  }

  ymir::Point2d<TileCord> toPos(const_iterator It) const {
    auto Dist = static_cast<TileCord>(std::distance(Data.begin(), It));
    return {Dist % Size.W, Dist / Size.W};
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