#ifndef YMIR_DEBUG_TILE_HPP
#define YMIR_DEBUG_TILE_HPP

#include <iostream>
#include <optional>
#include <ymir/Map.hpp>

namespace ymir {

template <typename TileType, typename OverrideType = char> struct DebugTile {
  inline constexpr explicit DebugTile() = default;
  inline constexpr explicit DebugTile(const TileType &Tile) : Tile(Tile) {}
  inline constexpr operator TileType() const { return Tile; }

  DebugTile &operator=(const OverrideType &OT) {
    PrintOverride = OT;
    return *this;
  }

  template <typename U>
  static ymir::Map<DebugTile<TileType, OverrideType>, U>
  convert(const Map<TileType, U> &M) {
    ymir::Map<DebugTile<TileType, OverrideType>, U> Converted(M.Size);
    M.forEach([&Converted](Point2d<U> Pos, const TileType &Tile) {
      Converted.setTile(Pos, DebugTile<TileType, OverrideType>(Tile));
    });
    return Converted;
  }

  TileType Tile;
  std::optional<OverrideType> PrintOverride;
};

template <typename TileType, typename OverrideType>
std::ostream &operator<<(std::ostream &Out,
                         const DebugTile<TileType, OverrideType> &DT) {
  if (DT.PrintOverride) {
    Out << *DT.PrintOverride;
  } else {
    Out << DT.Tile;
  }
  return Out;
}

} // namespace ymir

#endif // #ifndef YMIR_DEBUG_TILE_HPP