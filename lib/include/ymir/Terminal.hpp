#ifndef YMIR_TERMINAL_HPP
#define YMIR_TERMINAL_HPP

#include <cstring>
#include <iosfwd>
#include <stdint.h>
#include <tuple>
#include <variant>

namespace ymir {

struct NoColor {};
inline bool operator==(const NoColor &, const NoColor &) noexcept {
  return true;
}
inline bool operator!=(const NoColor &, const NoColor &) noexcept {
  return false;
}
std::ostream &operator<<(std::ostream &Out, const NoColor &NC);

struct DefaultColor {};
inline bool operator==(const DefaultColor &, const DefaultColor &) noexcept {
  return true;
}
inline bool operator!=(const DefaultColor &, const DefaultColor &) noexcept {
  return false;
}
std::ostream &operator<<(std::ostream &Out, const DefaultColor &DC);

struct RgbColor {
  static ymir::RgbColor getHeatMapColor(float Min, float Max, float Value);

  uint8_t R = 0;
  uint8_t G = 0;
  uint8_t B = 0;
  bool Foreground = true;
};
inline bool operator==(const RgbColor &Lhs, const RgbColor &Rhs) noexcept {
  return std::tie(Lhs.R, Lhs.G, Lhs.B, Lhs.Foreground) ==
         std::tie(Rhs.R, Rhs.G, Rhs.B, Rhs.Foreground);
}
inline bool operator!=(const RgbColor &Lhs, const RgbColor &Rhs) noexcept {
  return !(Lhs == Rhs);
}
std::ostream &operator<<(std::ostream &Out, const RgbColor &Color);

using TermColor = std::variant<NoColor, DefaultColor, RgbColor>;
std::ostream &operator<<(std::ostream &Out, const TermColor &TC);

struct UniChar {
  UniChar() = default;
  UniChar(const unsigned char Char[4]);
  UniChar &operator=(const unsigned char Char[4]);
  UniChar(std::array<unsigned char, 4> Char);
  UniChar &operator=(std::array<unsigned char, 4> Char);
  UniChar(char Char);
  UniChar &operator=(char Char);
  char Data[5] = {0};
};
inline bool operator==(const UniChar &Lhs, const UniChar &Rhs) noexcept {
  return std::memcmp(Lhs.Data, Rhs.Data, sizeof(Lhs.Data)) == 0;
}
inline bool operator!=(const UniChar &Lhs, const UniChar &Rhs) noexcept {
  return !(Lhs == Rhs);
}
std::ostream &operator<<(std::ostream &Out, const UniChar &Char);

struct ColoredUniChar {
  UniChar Char;
  TermColor Color = NoColor{};

  ColoredUniChar() = default;
  ColoredUniChar(char Char) : Char(Char) {}
  ColoredUniChar(UniChar Char) : Char(Char) {}
  ColoredUniChar(UniChar Char, TermColor Color) : Char(Char), Color(Color) {}
  ColoredUniChar &operator=(const UniChar &Char);
  ColoredUniChar &operator=(const TermColor &Color);
};
inline bool operator==(const ColoredUniChar &Lhs,
                       const ColoredUniChar &Rhs) noexcept {
  return Lhs.Char == Rhs.Char && Lhs.Color == Rhs.Color;
}
inline bool operator!=(const ColoredUniChar &Lhs,
                       const ColoredUniChar &Rhs) noexcept {
  return !(Lhs == Rhs);
}
std::ostream &operator<<(std::ostream &Out, const ColoredUniChar &Char);

} // namespace ymir

#endif // #ifndef YMIR_TERMINAL_HPP
