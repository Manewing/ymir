#include <iostream>
#include <unistd.h>
#include <ymir/Terminal.hpp>
#include <ymir/TypeHelpers.hpp>

namespace ymir {

std::ostream &operator<<(std::ostream &Out, const NoColor & /*NC*/) {
  return Out;
}

std::ostream &operator<<(std::ostream &Out, const DefaultColor & /*DC*/) {
  Out << "\033[0m";
  return Out;
}

ymir::RgbColor RgbColor::getHeatMapColor(float Min, float Max, float Value) {
  Value = std::max(Min, std::min(Value, Max));
  float Ratio = 2 * (Value - Min) / (Max - Min);
  int R = std::max(0, static_cast<int>(255 * (1 - Ratio)));
  int B = std::max(0, static_cast<int>(255 * (Ratio - 1)));
  int G = 255 - B - R;
  return {static_cast<uint8_t>(R), static_cast<uint8_t>(G),
          static_cast<uint8_t>(B)};
}

std::ostream &operator<<(std::ostream &Out, const RgbColor &Color) {
  // FIXME check if we have a tty, disable if we don't have a terminal
  Out << "\033[";
  if (Color.Foreground) {
    Out << "38";
  } else {
    Out << "48";
  }
  Out << ";2;" << static_cast<int>(Color.R) << ";" << static_cast<int>(Color.G)
      << ";" << static_cast<int>(Color.B) << "m";
  return Out;
}

std::ostream &operator<<(std::ostream &Out, const TermColor &TC) {
  std::visit(
      Overloaded{
          [&Out](const auto &C) { Out << C; },
      },
      TC);
  return Out;
}

UniChar::UniChar(const unsigned char Char[4]) {
  std::memcpy(Data, Char, sizeof(Data) - 1);
}

UniChar &UniChar::operator=(const unsigned char Char[4]) {
  std::memcpy(Data, Char, sizeof(Data) - 1);
  return *this;
}

UniChar::UniChar(std::array<unsigned char, 4> Char) {
  std::memcpy(Data, Char.data(), sizeof(Data) - 1);
}

UniChar &UniChar::operator=(std::array<unsigned char, 4> Char) {
  std::memcpy(Data, Char.data(), sizeof(Data) - 1);
  return *this;
}

UniChar::UniChar(char Char) {
  std::memset(Data, 0, sizeof(Data));
  Data[0] = Char;
}

UniChar &UniChar::operator=(char Char) {
  std::memset(Data, 0, sizeof(Data));
  Data[0] = Char;
  return *this;
}

std::ostream &operator<<(std::ostream &Out, const UniChar &Char) {
  Out << Char.Data;
  return Out;
}

ColoredUniChar &ColoredUniChar::operator=(const UniChar &Char) {
  this->Char = Char;
  return *this;
}

ColoredUniChar &ColoredUniChar::operator=(const TermColor &Color) {
  this->Color = Color;
  return *this;
}

std::ostream &operator<<(std::ostream &Out, const ColoredUniChar &Char) {

  Out << Char.Color << Char.Char;
  if (!std::holds_alternative<NoColor>(Char.Color)) {
    Out << DefaultColor{};
  }
  return Out;
}

} // namespace ymir