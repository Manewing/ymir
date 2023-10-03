#ifndef YMIR_TYPES_HPP
#define YMIR_TYPES_HPP

#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <ymir/Enum.hpp>

namespace ymir {

template <typename T> struct nd {
  typedef T type;
};

template <typename T> struct Size2d {
  using ValueType = T;
  ValueType W = 0;
  ValueType H = 0;

  constexpr Size2d() = default;
  constexpr Size2d(ValueType W, ValueType H) : W(W), H(H) {}

  inline Size2d &operator+=(const Size2d<T> &Rhs) {
    W += Rhs.W;
    H += Rhs.H;
    return *this;
  }
};

template <typename T>
inline bool operator==(const Size2d<T> &Lhs, const Size2d<T> &Rhs) noexcept {
  return Lhs.W == Rhs.W && Lhs.H == Rhs.H;
}

template <typename T>
inline bool operator!=(const Size2d<T> &Lhs, const Size2d<T> &Rhs) noexcept {
  return Lhs.W != Rhs.W || Lhs.H != Rhs.H;
}

template <typename T>
inline Size2d<T> operator+(const Size2d<T> &Lhs,
                           const Size2d<T> &Rhs) noexcept {
  return {Lhs.W + Rhs.W, Lhs.H + Rhs.H};
}

template <typename T> std::ostream &operator<<(std::ostream &Out, Size2d<T> S) {
  Out << "Size2d{" << S.W << ", " << S.H << "}";
  return Out;
}

template <typename T> struct Point2d {
  using ValueType = T;
  ValueType X = 0;
  ValueType Y = 0;

  constexpr Point2d() = default;
  constexpr Point2d(ValueType X, ValueType Y) : X(X), Y(Y) {}

  inline Point2d &operator+=(const Point2d<T> &Rhs) {
    X += Rhs.X;
    Y += Rhs.Y;
    return *this;
  }

  inline Point2d &operator-=(const Point2d<T> &Rhs) {
    X -= Rhs.X;
    Y -= Rhs.Y;
    return *this;
  }

  Point2d<ValueType> abs() const { return {std::abs(X), std::abs(Y)}; }
  Point2d<ValueType> positive() const { return {X < 0 ? 0 : X, Y < 0 ? 0 : Y}; }

  static ValueType manhattenDistance(const Point2d<ValueType> &A,
                                     const Point2d<ValueType> &B) {
    auto Diff = (A - B).abs();
    return Diff.X + Diff.Y;
  }

  /// Returns true if A is closer to Target than B
  static bool isCloser(const Point2d<ValueType> &A, const Point2d<ValueType> &B,
                       const Point2d<ValueType> &Target,
                       bool AllowEqual = false) {
    if (AllowEqual) {
      return manhattenDistance(A, Target) <= manhattenDistance(B, Target);
    }
    return manhattenDistance(A, Target) < manhattenDistance(B, Target);
  }
};

template <typename T>
inline bool operator==(const Point2d<T> &Lhs, const Point2d<T> &Rhs) noexcept {
  return std::tie(Lhs.X, Lhs.Y) == std::tie(Rhs.X, Rhs.Y);
}

template <typename T>
inline bool operator!=(const Point2d<T> &Lhs, const Point2d<T> &Rhs) noexcept {
  return std::tie(Lhs.X, Lhs.Y) != std::tie(Rhs.X, Rhs.Y);
}

template <typename T>
inline bool operator<(const Point2d<T> &Lhs, const Point2d<T> &Rhs) noexcept {
  return std::tie(Lhs.X, Lhs.Y) < std::tie(Rhs.X, Rhs.Y);
}

template <typename T>
inline bool operator<=(const Point2d<T> &Lhs, const Point2d<T> &Rhs) noexcept {
  return std::tie(Lhs.X, Lhs.Y) <= std::tie(Rhs.X, Rhs.Y);
}

template <typename T>
inline bool operator>(const Point2d<T> &Lhs, const Point2d<T> &Rhs) noexcept {
  return std::tie(Lhs.X, Lhs.Y) > std::tie(Rhs.X, Rhs.Y);
}

template <typename T>
inline bool operator>=(const Point2d<T> &Lhs, const Point2d<T> &Rhs) noexcept {
  return std::tie(Lhs.X, Lhs.Y) >= std::tie(Rhs.X, Rhs.Y);
}

template <typename T>
inline Point2d<T> operator+(const Point2d<T> &Lhs,
                            const Point2d<T> &Rhs) noexcept {
  return {Lhs.X + Rhs.X, Lhs.Y + Rhs.Y};
}

template <typename T>
inline Point2d<T> operator-(const Point2d<T> &Lhs,
                            const Point2d<T> &Rhs) noexcept {
  return {Lhs.X - Rhs.X, Lhs.Y - Rhs.Y};
}

template <typename T>
inline Point2d<T> operator+(const Point2d<T> &Lhs,
                            const Size2d<T> &Rhs) noexcept {
  return {Lhs.X + Rhs.W, Lhs.Y + Rhs.H};
}

template <typename T>
inline Point2d<T> operator+(const Size2d<T> &Lhs,
                            const Point2d<T> &Rhs) noexcept {
  return Rhs + Lhs;
}

template <typename T>
inline Point2d<T> operator-(const Point2d<T> &Lhs,
                            const Size2d<T> &Rhs) noexcept {
  return {Lhs.X - Rhs.W, Lhs.Y - Rhs.H};
}

template <typename T>
std::ostream &operator<<(std::ostream &Out, Point2d<T> P) {
  Out << "Point2d{" << P.X << ", " << P.Y << "}";
  return Out;
}

// 2D Rectangle
//  x ->
// y             Size.W
// |  Pos -> +-------------+
// v         |             |
//           |             | Size.H
//           |             |
//           +-------------+
//
template <typename T> struct Rect2d {
  /// Top-left (in screen coordinates) position of the rectangle
  Point2d<T> Pos;

  /// Size of the rectangle
  Size2d<T> Size;

  constexpr Rect2d() = default;
  constexpr Rect2d(Point2d<T> Pos, Size2d<T> Size) : Pos(Pos), Size(Size) {}
  constexpr Rect2d(Size2d<T> Size) : Pos(0, 0), Size(Size) {}

  bool empty() const { return Size.W <= 0 || Size.H <= 0; }

  // Returns true if the given point is contained in the rect
  bool contains(const Point2d<T> &P) const {
    const auto Pos2 = Pos + Size;
    return Pos.X <= P.X && P.X < Pos2.X && Pos.Y <= P.Y && P.Y < Pos2.Y;
  }

  // Returns true if this rect completely contains the other rect
  bool contains(const Rect2d<T> &Other) const {
    const auto Pos2 = Pos + Size;
    const auto OtherPos2 = Other.Pos + Other.Size;
    return contains(Other.Pos) &&
           (Pos.X <= OtherPos2.X && OtherPos2.X <= Pos2.X &&
            Pos.Y <= OtherPos2.Y && OtherPos2.Y <= Pos2.Y);
  }

  // Returns true if the other rect overlaps with this rect
  bool overlaps(const Rect2d<T> &Other) const {
    const auto ThisPos = Pos + Size;
    const auto OtherPos = Other.Pos + Other.Size;
    return (Pos.X < OtherPos.X && ThisPos.X > Other.Pos.X &&
            Pos.Y < OtherPos.Y && ThisPos.Y > Other.Pos.Y);
  }

  static Rect2d<T> get(Point2d<T> Pos1, Point2d<T> Pos2) {
    auto Pos = Point2d<T>(std::min(Pos1.X, Pos2.X), std::min(Pos1.Y, Pos2.Y));
    const auto BottomRight =
        Point2d<T>(std::max(Pos1.X, Pos2.X), std::max(Pos1.Y, Pos2.Y));
    const auto Diff = BottomRight - Pos;
    return {Pos, {Diff.X, Diff.Y}};
  }
};

template <typename T>
inline bool operator==(const Rect2d<T> &Lhs, const Rect2d<T> &Rhs) noexcept {
  return std::tie(Lhs.Pos, Lhs.Size) == std::tie(Rhs.Pos, Rhs.Size);
}

template <typename T>
inline bool operator!=(const Rect2d<T> &Lhs, const Rect2d<T> &Rhs) noexcept {
  return std::tie(Lhs.Pos, Lhs.Size) != std::tie(Rhs.Pos, Rhs.Size);
}

template <typename T, typename U>
inline Rect2d<T> operator&(const Rect2d<T> &Lhs, const Rect2d<U> &Rhs) {

  // Determine top left
  const Point2d<T> Pos1{std::max(Lhs.Pos.X, Rhs.Pos.X),
                        std::max(Lhs.Pos.Y, Rhs.Pos.Y)};
  // Determine bottom right
  const Point2d<T> Pos2{
      std::min(Lhs.Pos.X + Lhs.Size.W, Rhs.Pos.X + Rhs.Size.W),
      std::min(Lhs.Pos.Y + Lhs.Size.H, Rhs.Pos.Y + Rhs.Size.H)};

  // Get width and height
  const Point2d<T> Diff = Pos2 - Pos1;
  if (Diff.X <= 0 || Diff.Y <= 0) {
    return Rect2d<T>();
  }
  const Size2d<T> Size{Diff.X, Diff.Y};
  return {Pos1, Size};
}

template <typename T>
std::ostream &operator<<(std::ostream &Out, Rect2d<T> Rect) {
  Out << "Rect2d{" << Rect.Pos << ", " << Rect.Size << "}";
  return Out;
}

class Dir2d {
public:
  typedef enum : char {
    NONE = 0x00,
    DOWN = 0x01,
    UP = 0x02,
    RIGHT = 0x04,
    LEFT = 0x08,
    VERTICAL = DOWN | UP,
    HORIZONTAL = LEFT | RIGHT
  } Dir2dValue;

  static Dir2d fromString(std::string Str);

  /// Returns the direction of the maximum component (absolute) of (X,Y) as a
  /// single direction
  static Dir2d fromMaxComponent(const int X, const int Y);

  /// Returns the direction of the maximum component (absolute) of P as a single
  /// direction
  template <typename T> static Dir2d fromMaxComponent(const Point2d<T> &P) {
    return fromVector(P.X, P.Y);
  }

  /// Returns the direction of the vector (X,Y) as a composite direction (e.g.
  /// UP | RIGHT)
  static Dir2d fromVector(const int X, const int Y);

  /// Returns the direction of the vector P as a composite direction (e.g. UP |
  /// RIGHT)
  template <typename T> static Dir2d fromVector(const Point2d<T> &P) {
    return fromVector(P.X, P.Y);
  }

  Dir2d() = default;
  inline constexpr Dir2d(Dir2dValue Value) : Value(Value) {}
  inline constexpr operator Dir2dValue() const { return Value; }

  std::string str() const;

  Dir2d &operator|=(Dir2d Other);
  Dir2d &operator&=(Dir2d Other);
  Dir2d &operator^=(Dir2d Other);

  Dir2d opposing() const;

  inline constexpr bool isHorizontal() const {
    return (Value & HORIZONTAL) != NONE;
  }

  inline constexpr bool isVertical() const {
    return (Value & VERTICAL) != NONE;
  }

  template <typename U>
  Point2d<U> advance(Point2d<U> Pos = Point2d<U>{0, 0}) const {
    if ((Value & DOWN) != NONE) {
      Pos += Point2d<U>{0, 1};
    }
    if ((Value & UP) != NONE) {
      Pos += Point2d<U>{0, -1};
    }
    if ((Value & RIGHT) != NONE) {
      Pos += Point2d<U>{1, 0};
    }
    if ((Value & LEFT) != NONE) {
      Pos += Point2d<U>{-1, 0};
    }
    return Pos;
  }

private:
  Dir2dValue Value = NONE;
};

std::ostream &operator<<(std::ostream &Out, const Dir2d &Dir);

template <typename U> Point2d<U> operator+(const Point2d<U> &P, Dir2d Dir) {
  return Dir.advance(P);
}

template <typename U> Point2d<U> &operator+=(Point2d<U> &P, Dir2d Dir) {
  P = Dir.advance(P);
  return P;
}

template <typename U> Point2d<U> operator+(Dir2d Dir, const Point2d<U> &P) {
  return Dir.advance(P);
}

template <typename U> Point2d<U> operator-(const Point2d<U> &P, Dir2d Dir) {
  return Dir.opposing().advance(P);
}

template <typename TileCord, typename Derived> struct TileDirections {
  template <typename MapType, typename BinaryFunc>
  static inline void forEach(MapType &Map, ymir::Point2d<TileCord> Start,
                             BinaryFunc Func) {
    for (const auto &Direction : Derived::get()) {
      auto Pos = Start + Direction;
      if (!Map.contains(Pos)) {
        continue;
      }
      auto &Tile = Map.getTile(Pos);
      if (!Func(Pos, Tile)) {
        return;
      }
    }
  }
};

///
///   x
///  x@x
///   x
///
template <typename TileCord>
struct FourTileDirections
    : public TileDirections<TileCord, FourTileDirections<TileCord>> {
  static constexpr std::array Directions = {
      ymir::Point2d<TileCord>{-1, 0},
      ymir::Point2d<TileCord>{1, 0},
      ymir::Point2d<TileCord>{0, -1},
      ymir::Point2d<TileCord>{0, 1},
  };
  static constexpr auto &get() { return Directions; }

  static constexpr bool isNextTo(const ymir::Point2d<TileCord> &A,
                                 const ymir::Point2d<TileCord> &B) {
    auto Diff = (A - B).abs();
    return (Diff.X == 1 && Diff.Y == 0) || (Diff.X == 0 && Diff.Y == 1);
  }
};

///
///  xxx
///  x@x
///  xxx
///
template <typename TileCord>
struct EightTileDirections
    : public TileDirections<TileCord, EightTileDirections<TileCord>> {
  static constexpr std::array Directions = {
      ymir::Point2d<TileCord>{-1, 0}, ymir::Point2d<TileCord>{1, 0},
      ymir::Point2d<TileCord>{0, -1}, ymir::Point2d<TileCord>{0, 1},
      ymir::Point2d<TileCord>{-1, 1}, ymir::Point2d<TileCord>{-1, -1},
      ymir::Point2d<TileCord>{1, 1},  ymir::Point2d<TileCord>{1, -1},
  };
  static constexpr auto &get() { return Directions; }

  static constexpr bool isNextTo(const ymir::Point2d<TileCord> &A,
                                 const ymir::Point2d<TileCord> &B) {
    auto Diff = (A - B).abs();
    return Diff.X <= 1 && Diff.Y <= 1 && (Diff.X != 0 || Diff.Y != 0);
  }
};

} // namespace ymir

namespace std {
// Hash for Point2d
template <typename TileCord> struct hash<::ymir::Point2d<TileCord>> {
  std::size_t operator()(const ::ymir::Point2d<TileCord> &P) const noexcept {
    std::hash<TileCord> Hasher;
    auto HX = Hasher(P.X);
    auto HY = Hasher(P.Y);
    return HX ^ (HY << 1);
  }
};
} // namespace std

YMIR_BITFIELD_ENUM(ymir::Dir2d::Dir2dValue);

#endif // #ifndef YMIR_TYPES_HPP
