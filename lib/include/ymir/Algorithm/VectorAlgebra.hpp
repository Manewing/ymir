#ifndef YMIR_ALGORITHM_VECTOR_ALGEBRA_HPP
#define YMIR_ALGORITHM_VECTOR_ALGEBRA_HPP

#include <algorithm>
#include <functional>
#include <tuple>
#include <vector>

// Functionality that allows to do algebra with vectors, for example:
//
//    const std::vector<int> ValuesA = {0, 1, 2, 3, 4};
//    const std::vector<int> ValuesB = {5, 6, 7, 8, 9};
//    std::vector<int> ValuesC = {0, 0, 0, 0, 0};
//
//    auto [VA, VB, VC] = ymir::Algorithm::algebra(ValuesA, ValuesB, ValuesC);
//    VC = VA / 2 + VB * 3 + 5;
//
//    assert(ValuesC == std::vector<int>{20, 23, 27, 30, 34});
//

namespace ymir::Algorithm {

namespace internal {
/// Just like std::divides, but flips "x / y" to "y / x"
template <typename T> struct InverseDivides : std::binary_function<T, T, T> {
  constexpr T operator()(const T &X, const T &Y) const { return Y / X; }
};

/// Just like std::minus, but flips "x - y" to "y - x"
template <typename T> struct InverseMinus : std::binary_function<T, T, T> {
  constexpr T operator()(const T &X, const T &Y) const { return Y - X; }
};

template <typename T, typename BinaryOp, typename Iterator>
struct ScalarIterator {
public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = typename Iterator::difference_type;
  using value_type = T;
  using pointer = T;
  using reference = T;

  explicit ScalarIterator(Iterator It) : It(It), Value() {}
  explicit ScalarIterator(Iterator It, T Value) : It(It), Value(Value) {}

  inline T operator*() const { return BinaryOp()(*It, Value); }

  inline ScalarIterator &operator++() {
    It++;
    return *this;
  }

  inline ScalarIterator operator++(int) {
    ScalarIterator Copy = *this;
    ++(*this);
    return Copy;
  }

  Iterator It;
  T Value;
};

template <typename T, typename BinaryOp, typename Iter>
inline bool operator==(const ScalarIterator<T, BinaryOp, Iter> &Lhs,
                       const ScalarIterator<T, BinaryOp, Iter> &Rhs) noexcept {
  return Lhs.It == Rhs.It;
}
template <typename T, typename BinaryOp, typename Iter>
inline bool operator!=(const ScalarIterator<T, BinaryOp, Iter> &Lhs,
                       const ScalarIterator<T, BinaryOp, Iter> &Rhs) noexcept {
  return !(Lhs == Rhs);
}

template <typename T, typename BinaryOp>
struct VectorScalarIterator
    : public ScalarIterator<T, BinaryOp,
                            typename std::vector<T>::const_iterator> {
public:
  using Container = std::vector<T>;
  using BaseIterator = typename Container::const_iterator;
  using ScalarIterator<T, BinaryOp, BaseIterator>::ScalarIterator;
};

template <typename T, typename BinaryOp, typename IteratorA, typename IteratorB>
struct ZipIterator {
public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = typename IteratorA::difference_type;
  using value_type = T;
  using pointer = T;
  using reference = T;

  explicit ZipIterator(IteratorA ItA, IteratorB ItB) : ItA(ItA), ItB(ItB) {}

  inline T operator*() const { return BinaryOp()(*ItA, *ItB); }

  inline ZipIterator &operator++() {
    ItA++;
    ItB++;
    return *this;
  }

  inline ZipIterator operator++(int) {
    ZipIterator Copy = *this;
    ++(*this);
    return Copy;
  }

  IteratorA ItA;
  IteratorB ItB;
};

template <typename T, typename BinaryOp, typename IterA, typename IterB>
inline bool
operator==(const ZipIterator<T, BinaryOp, IterA, IterB> &Lhs,
           const ZipIterator<T, BinaryOp, IterA, IterB> &Rhs) noexcept {
  return Lhs.ItA == Rhs.ItA && Lhs.ItB == Rhs.ItB;
}

template <typename T, typename BinaryOp, typename IterA, typename IterB>
inline bool
operator!=(const ZipIterator<T, BinaryOp, IterA, IterB> &Lhs,
           const ZipIterator<T, BinaryOp, IterA, IterB> &Rhs) noexcept {
  return !(Lhs == Rhs);
}

template <typename T, typename BinaryOp>
struct VectorZipIterator
    : public ZipIterator<T, BinaryOp, typename std::vector<T>::const_iterator,
                         typename std::vector<T>::const_iterator> {
public:
  using Container = std::vector<T>;
  using BaseIterator = typename Container::const_iterator;
  using ZipIterator<T, BinaryOp, BaseIterator, BaseIterator>::ZipIterator;
};

template <typename T, typename Iterator> struct AlgebraView {
  Iterator Begin;
  Iterator End;
  const Iterator &begin() const { return Begin; }
  const Iterator &end() const { return End; }
};

// AlgebraView computation functions
template <typename T, typename BinaryOp, typename IterA, typename IterB>
inline auto opViews(const AlgebraView<T, IterA> &Lhs,
                    const AlgebraView<T, IterB> &Rhs) {
  using Iterator = ZipIterator<T, BinaryOp, IterA, IterB>;
  return AlgebraView<T, Iterator>{Iterator(Lhs.begin(), Rhs.begin()),
                                  Iterator(Lhs.end(), Rhs.end())};
}

template <typename T, typename BinaryOp, typename Iter>
inline auto opViewScalar(const AlgebraView<T, Iter> &AV, const T &Value) {
  using Iterator = ScalarIterator<T, BinaryOp, Iter>;
  return AlgebraView<T, Iterator>{Iterator(AV.begin(), Value),
                                  Iterator(AV.end())};
}

// AlgebraView binary operators (AV, AV)
template <typename T, typename IterA, typename IterB>
inline auto operator/(const AlgebraView<T, IterA> &Lhs,
                      const AlgebraView<T, IterB> &Rhs) {
  return opViews<T, std::divides<T>, IterA, IterB>(Lhs, Rhs);
}

template <typename T, typename IterA, typename IterB>
inline auto operator*(const AlgebraView<T, IterA> &Lhs,
                      const AlgebraView<T, IterB> &Rhs) {
  return opViews<T, std::multiplies<T>, IterA, IterB>(Lhs, Rhs);
}

template <typename T, typename IterA, typename IterB>
inline auto operator+(const AlgebraView<T, IterA> &Lhs,
                      const AlgebraView<T, IterB> &Rhs) {
  return opViews<T, std::plus<T>, IterA, IterB>(Lhs, Rhs);
}

template <typename T, typename IterA, typename IterB>
inline auto operator-(const AlgebraView<T, IterA> &Lhs,
                      const AlgebraView<T, IterB> &Rhs) {
  return opViews<T, std::minus<T>, IterA, IterB>(Lhs, Rhs);
}

// AlgebraView binary operators (AV, Value), (Value, AV)
template <typename T, typename Iter>
inline auto operator/(const AlgebraView<T, Iter> &AV, const T &Value) {
  return opViewScalar<T, std::divides<T>, Iter>(AV, Value);
}

template <typename T, typename Iter>
inline auto operator/(const T &Value, const AlgebraView<T, Iter> &AV) {
  return opViewScalar<T, InverseDivides<T>, Iter>(AV, Value);
}

template <typename T, typename Iter>
inline auto operator*(const AlgebraView<T, Iter> &AV, const T &Value) {
  return opViewScalar<T, std::multiplies<T>, Iter>(AV, Value);
}

template <typename T, typename Iter>
inline auto operator*(const T &Value, const AlgebraView<T, Iter> &AV) {
  // symmetric
  return AV * Value;
}

template <typename T, typename Iter>
inline auto operator+(const AlgebraView<T, Iter> &AV, const T &Value) {
  return opViewScalar<T, std::plus<T>, Iter>(AV, Value);
}

template <typename T, typename Iter>
inline auto operator+(const T &Value, const AlgebraView<T, Iter> &AV) {
  // symmetric
  return AV + Value;
}

template <typename T, typename Iter>
inline auto operator-(const AlgebraView<T, Iter> &AV, const T &Value) {
  return opViewScalar<T, std::minus<T>, Iter>(AV, Value);
}

template <typename T, typename Iter>
inline auto operator-(const T &Value, const AlgebraView<T, Iter> &AV) {
  return opViewScalar<T, InverseMinus<T>, Iter>(AV, Value);
}

// Vector transformation functions: (vector, scalar), (vector, vector)
template <typename T, typename BinaryOp>
void scalarOp(std::vector<T> &Vals, const T &Value) {
  std::transform(Vals.begin(), Vals.end(), Vals.begin(),
                 std::bind(BinaryOp(), std::placeholders::_1, Value));
}

template <typename T, typename BinaryOp>
void opVals(std::vector<T> &Lhs, const std::vector<T> &Rhs) {
  std::transform(Lhs.begin(), Lhs.end(), Rhs.begin(), Lhs.begin(), BinaryOp());
}

// Vector view transformation functions: (vector, scalar), (vector, vector)
template <typename T, typename BinaryOp>
auto scalarOpView(const std::vector<T> &Vals, const T &Value) {
  using Iterator = VectorScalarIterator<T, BinaryOp>;
  return AlgebraView<T, Iterator>{Iterator(Vals.begin(), Value),
                                  Iterator(Vals.end())};
}

template <typename T, typename BinaryOp>
auto opValsView(const std::vector<T> &Lhs, const std::vector<T> &Rhs) {
  using Iterator = VectorZipIterator<T, BinaryOp>;
  return AlgebraView<T, Iterator>{Iterator(Lhs.begin(), Rhs.begin()),
                                  Iterator(Lhs.end(), Rhs.end())};
}

template <typename VectorType> struct AlgebraWrapperBase {
  using ValueType = typename VectorType::value_type;
  explicit AlgebraWrapperBase(VectorType &Vals) : Vals(Vals) {}
  VectorType &Vals;
};

template <typename T>
struct ConstAlgebraWrapper : public AlgebraWrapperBase<const std::vector<T>> {
  using VectorType = const std::vector<T>;
  using AlgebraWrapperBase<VectorType>::AlgebraWrapperBase;
};

template <typename T>
struct AlgebraWrapper : public AlgebraWrapperBase<std::vector<T>> {
  using VectorType = std::vector<T>;
  using AlgebraWrapperBase<VectorType>::AlgebraWrapperBase;

  template <typename U> AlgebraWrapper &operator=(const AlgebraView<T, U> &AV) {
    std::copy(AV.begin(), AV.end(), this->Vals.begin());
    return *this;
  }

  AlgebraWrapper &operator*=(const T &Value) {
    scalarOp<T, std::multiplies<T>>(this->Vals, Value);
    return *this;
  }

  AlgebraWrapper &operator/=(const T &Value) {
    scalarOp<T, std::divides<T>>(this->Vals, Value);
    return *this;
  }

  AlgebraWrapper &operator+=(const T &Value) {
    scalarOp<T, std::plus<T>>(this->Vals, Value);
    return *this;
  }

  AlgebraWrapper &operator-=(const T &Value) {
    scalarOp<T, std::minus<T>>(this->Vals, Value);
    return *this;
  }
};

// AlgebraWrapper binary operators: (AW, AW)
template <typename U>
inline auto operator/(const AlgebraWrapperBase<U> &Lhs,
                      const AlgebraWrapperBase<U> &Rhs) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return opValsView<T, std::divides<T>>(Lhs.Vals, Rhs.Vals);
}

template <typename U>
inline auto operator*(const AlgebraWrapperBase<U> &Lhs,
                      const AlgebraWrapperBase<U> &Rhs) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return opValsView<T, std::multiplies<T>>(Lhs.Vals, Rhs.Vals);
}

template <typename U>
inline auto operator+(const AlgebraWrapperBase<U> &Lhs,
                      const AlgebraWrapperBase<U> &Rhs) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return opValsView<T, std::plus<T>>(Lhs.Vals, Rhs.Vals);
}

template <typename U>
inline auto operator-(const AlgebraWrapperBase<U> &Lhs,
                      const AlgebraWrapperBase<U> &Rhs) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return opValsView<T, std::minus<T>>(Lhs.Vals, Rhs.Vals);
}

// AlgebraWrapperBase binary operators: (AW, Value), (Value, AW)
template <typename U>
inline auto operator/(const AlgebraWrapperBase<U> &AW,
                      const typename AlgebraWrapperBase<U>::ValueType &Value) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return scalarOpView<T, std::divides<T>>(AW.Vals, Value);
}

template <typename U>
inline auto operator/(const typename AlgebraWrapperBase<U>::ValueType &Value,
                      const AlgebraWrapperBase<U> &AW) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return scalarOpView<T, InverseDivides<T>>(AW.Vals, Value);
}

template <typename U>
inline auto operator*(const AlgebraWrapperBase<U> &AW,
                      const typename AlgebraWrapperBase<U>::ValueType &Value) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return scalarOpView<T, std::multiplies<T>>(AW.Vals, Value);
}

template <typename U>
inline auto operator*(const typename AlgebraWrapperBase<U>::ValueType &Value,
                      const AlgebraWrapperBase<U> &AW) {
  // symmetric
  return AW * Value;
}

template <typename U>
inline auto operator+(const AlgebraWrapperBase<U> &AW,
                      const typename AlgebraWrapperBase<U>::ValueType &Value) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return scalarOpView<T, std::plus<T>>(AW.Vals, Value);
}

template <typename U>
inline auto operator+(const typename AlgebraWrapperBase<U>::ValueType &Value,
                      const AlgebraWrapperBase<U> &AW) {
  // symmetric
  return AW + Value;
}

template <typename U>
inline auto operator-(const AlgebraWrapperBase<U> &AW,
                      const typename AlgebraWrapperBase<U>::ValueType &Value) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return scalarOpView<T, std::minus<T>>(AW.Vals, Value);
}

template <typename U>
inline auto operator-(const typename AlgebraWrapperBase<U>::ValueType &Value,
                      const AlgebraWrapperBase<U> &AW) {
  using T = typename AlgebraWrapperBase<U>::ValueType;
  return scalarOpView<T, InverseMinus<T>>(AW.Vals, Value);
}

} // namespace internal

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