#ifndef YMIR_MAP_HPP
#define YMIR_MAP_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <iterator>
#include <optional>
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
  using reference = typename DataType::reference;
  using const_reference = typename DataType::const_reference;

  template <typename TX, typename UX>
  friend bool operator==(const Map<TX, UX> &Lhs, const Map<TX, UX> &Rhs);

public:
  Map() = default;

  // TODO allow passing tile for default value when resizing
  explicit Map(Size2d<TileCord> Size) : Size(Size) { resize(Size); }

  Map(TileCord Width, TileCord Height) : Map(Size2d<TileCord>{Width, Height}) {}

  Size2d<TileCord> getSize() const { return Size; }

  bool empty() const { return Data.empty(); }

  void resize(Size2d<TileCord> Size) {
    this->Size = Size;
    Data.resize(Size.W * Size.H);
  }

  inline void setTile(TilePos P, TileType Tile) { getTile(P) = Tile; }

  void setTiles(const std::vector<ymir::Point2d<TileCord>> &TilePos,
                TileType Tile) {
    for (auto const &Pos : TilePos) {
      setTile(Pos, Tile);
    }
  }

  inline reference getTile(TilePos P) { return Data.at(P.X + P.Y * Size.W); }

  inline const_reference getTile(TilePos P) const {
    return Data.at(P.X + P.Y * Size.W);
  }

  inline bool isTile(TilePos P, TileType Tile) const {
    if (static_cast<unsigned>(P.X + P.Y * Size.W) >= Data.size()) {
      return false;
    }
    return getTile(P) == Tile;
  }

  /// Iterates over neighbors of the current position that are within the bounds
  // of the map, calls binary function for each position and tile.
  template <typename BinaryFunction,
            typename DirectionProvider = EightTileDirections<TileCord>>
  void checkNeighbors(
      TilePos P, BinaryFunction Func,
      [[maybe_unused]] DirectionProvider DirProv = DirectionProvider()) const {
    DirectionProvider::forEach(*this, P, Func);
  }

  template <typename DirectionProvider = EightTileDirections<TileCord>>
  std::size_t getNeighborCount(
      TilePos P, TileType Tile,
      [[maybe_unused]] DirectionProvider DirProv = DirectionProvider()) const {
    std::size_t Count = 0;
    checkNeighbors(
        P,
        [Tile, &Count](auto, auto CT) {
          if (CT == Tile) {
            Count++;
          }
          return true;
        },
        DirProv);
    return Count;
  }

  template <typename DirectionProvider = EightTileDirections<TileCord>>
  std::size_t getNotNeighborCount(
      TilePos P, TileType Tile,
      [[maybe_unused]] DirectionProvider DirProv = DirectionProvider()) const {
    return DirectionProvider::Directions.size() -
           getNeighborCount(P, Tile, DirProv);
  }

  inline constexpr Rect2d<TileCord> rect() const {
    return Rect2d<TileCord>{{0, 0}, Size};
  }

  inline constexpr bool contains(Point2d<TileCord> P) const {
    return rect().contains(P);
  }

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

  std::vector<ymir::Point2d<TileCord>> findTilesNot(TileType Target) const {
    std::vector<ymir::Point2d<TileCord>> Result;
    forEach([&Target, &Result](auto Pos, const auto &Tile) {
      if (Target != Tile) {
        Result.push_back(Pos);
      }
    });
    return Result;
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

  std::vector<TileType> &getData() { return Data; }
  const std::vector<TileType> &getData() const { return Data; }

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