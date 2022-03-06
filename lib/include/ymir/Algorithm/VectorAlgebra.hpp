#ifndef YMIR_ALGORITHM_VECTOR_ALGEBRA_HPP
#define YMIR_ALGORITHM_VECTOR_ALGEBRA_HPP

#include <tuple>
#include <vector>
#include <ymir/Algorithm/VectorAlgebraInternal.hpp>

// Functionality that allows to do algebra with vectors, for example:
//
//    std::vector<int> ValuesA = {0, 1, 2, 3, 4};
//    const std::vector<int> ValuesB = {5, 6, 7, 8, 9};
//
//    auto [VA, VB] = ymir::Algorithm::algebra(ValuesA, ValuesB);
//    VA = VA / 2 + VB * 3 + 5;
//
//    assert(ValuesA == std::vector<int>{20, 23, 27, 30, 34});
//

namespace ymir::Algorithm {

template <typename T> auto algebra(const std::vector<T> &Vals) {
  return internal::ConstAlgebraWrapper<T>(Vals);
}

template <typename T> auto algebra(std::vector<T> &Vals) {
  return internal::AlgebraWrapper<T>(Vals);
}

template <typename... ArgTypes> auto algebra(ArgTypes &&... Args) {
  return std::tuple(algebra(Args)...);
}

} // namespace ymir::Algorithm

#endif // #ifndef YMIR_ALGORITHM_VECTOR_ALGEBRA_HPP