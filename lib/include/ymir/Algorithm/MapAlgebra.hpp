#ifndef YMIR_ALGORITHM_MAP_ALGEBRA_HPP
#define YMIR_ALGORITHM_MAP_ALGEBRA_HPP

#include <ymir/Algorithm/VectorAlgebraInternal.hpp>
#include <ymir/Map.hpp>

// Functionality that allows to do algebra with ymir::Maps, for example:
//
//    ymir::Map<int, int> MapA; // Data == {0, 1, 2, 3, 4};
//    const ymir::Map<int, int> MapB; // Data == {5, 6, 7, 8, 9};
//
//    auto [MA, MB] = ymir::Algorithm::algebra(MapA, MapB);
//    MC = MA / 2 + MB * 3 + 5;
//
//    assert(MapA.getData() == {20, 23, 27, 30, 34});
//

namespace ymir::Algorithm {

template <typename TileType, typename TileCord>
auto algebra(const ymir::Map<TileType, TileCord> &M) {
  return internal::ConstAlgebraWrapper<TileType>(M.getData());
}

template <typename TileType, typename TileCord>
auto algebra(ymir::Map<TileType, TileCord> &M) {
  return internal::AlgebraWrapper<TileType>(M.getData());
}

} // namespace ymir::Algorithm

#include <ymir/Algorithm/VectorAlgebra.hpp>

#endif // #ifndef YMIR_ALGORITHM_MAP_ALGEBRA_HPP