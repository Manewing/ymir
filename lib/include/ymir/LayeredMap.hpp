#ifndef YMIR_LAYERED_MAP_HPP
#define YMIR_LAYERED_MAP_HPP

#include <string>
#include <vector>
#include <ymir/Map.hpp>

namespace ymir {

template <typename TileType, typename TileCord = int> class LayeredMap {
public:
  using MapType = Map<TileType, TileCord>;

public:
  LayeredMap() = default;

  LayeredMap(std::size_t NumLayers, Size2d<TileCord> Size)
      : Size(Size), Layers(), Names() {
    Layers.resize(NumLayers, MapType(Size));
    Names.resize(NumLayers, "");
  }

  LayeredMap(std::size_t NumLayers, TileCord Width, TileCord Height)
      : LayeredMap(NumLayers, {Width, Height}) {}

  LayeredMap(std::vector<std::string> LayerNames, Size2d<TileCord> Size)
      : LayeredMap(LayerNames.size(), Size) {
    Names = std::move(LayerNames);
  }

  inline Size2d<TileCord> getSize() const { return Size; }
  std::size_t getNumLayers() const { return Layers.size(); }
  Rect2d<TileCord> rect() const { return Rect2d<TileCord>{{0, 0}, Size}; }
  bool contains(Point2d<TileCord> P) const { return rect().contains(P); }

  MapType &get(std::size_t LayerIdx) { return Layers.at(LayerIdx); }
  const MapType &get(std::size_t LayerIdx) const { return Layers.at(LayerIdx); }
  MapType &get(const std::string &LayerName);
  const MapType &get(const std::string &LayerName) const;

  const std::vector<MapType> &getLayers() const { return Layers; }

  MapType render(TileType Transparent = TileType()) const;

private:
  Size2d<TileCord> Size;
  std::vector<MapType> Layers;
  std::vector<std::string> Names;
};

template <typename T, typename U>
Map<T, U> &LayeredMap<T, U>::get(const std::string &LayerName) {
  return const_cast<Map<T, U> &>(
      static_cast<const LayeredMap<T, U> *>(this)->get(LayerName));
}

template <typename T, typename U>
const Map<T, U> &LayeredMap<T, U>::get(const std::string &LayerName) const {
  auto It = std::find(Names.begin(), Names.end(), LayerName);
  if (It != Names.end()) {
    return Layers.at(It - Names.begin());
  }
  throw std::out_of_range("Could not find layer '" + LayerName + "'");
}

template <typename T, typename U>
Map<T, U> LayeredMap<T, U>::render(T Transparent) const {
  Map<T, U> Map(Size);
  Map.fill(Transparent);
  for (auto const &Layer : Layers) {
    Layer.forEach([Transparent, &Map](Point2d<U> Pos, T Tile) {
      if (Tile == Transparent) {
        return;
      }
      Map.setTile(Pos, Tile);
    });
  }
  return Map;
}

} // namespace ymir

#endif // #ifndef YMIR_LAYERED_MAP_HPP