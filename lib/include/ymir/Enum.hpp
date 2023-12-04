#ifndef YMIR_ENUM_HPP
#define YMIR_ENUM_HPP

#include <type_traits>

#define YMIR_BITFIELD_ENUM(T)                                                  \
  template <> struct _ymir_bitfield_enum<T> {                                  \
    static const bool enabled = true;                                          \
  };

template <typename T> struct _ymir_bitfield_enum {
  static const bool enabled = false;
};

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type
operator|(T Lhs, T Rhs) {
  using U = typename std::underlying_type<T>::type;
  return static_cast<T>(static_cast<U>(Lhs) | static_cast<U>(Rhs));
}

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type
operator&(T Lhs, T Rhs) {
  using U = typename std::underlying_type<T>::type;
  return static_cast<T>(static_cast<U>(Lhs) & static_cast<U>(Rhs));
}

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type
operator^(T Lhs, T Rhs) {
  using U = typename std::underlying_type<T>::type;
  return static_cast<T>(static_cast<U>(Lhs) ^ static_cast<U>(Rhs));
}

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type
operator~(T Rhs) {
  using U = typename std::underlying_type<T>::type;
  return static_cast<T>(~static_cast<U>(Rhs));
}

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type &
operator|=(T &Lhs, const T &Rhs) {
  using U = typename std::underlying_type<T>::type;
  Lhs = static_cast<T>(static_cast<U>(Lhs) | static_cast<U>(Rhs));
  return Lhs;
}

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type &
operator&=(T &Lhs, const T &Rhs) {
  using U = typename std::underlying_type<T>::type;
  Lhs = static_cast<T>(static_cast<U>(Lhs) & static_cast<U>(Rhs));
  return Lhs;
}

template <typename T>
constexpr typename std::enable_if<_ymir_bitfield_enum<T>::enabled, T>::type &
operator^=(T &Lhs, const T &Rhs) {
  using U = typename std::underlying_type<T>::type;
  Lhs = static_cast<T>(static_cast<U>(Lhs) ^ static_cast<U>(Rhs));
  return Lhs;
}

#endif // #ifndef YMIR_ENUM_HPP