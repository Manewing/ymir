#include <ymir/Config/Types.hpp>

namespace ymir::Config {

Dir2d parseDir2d(const std::string &Value) {
  static const std::regex Regex("(Dir2d\\()?([A-Z |]+)(\\))?");
  std::smatch Match;
  if (std::regex_match(Value, Match, Regex)) {
    return Dir2d::fromString(Match[2]);
  }
  throw std::runtime_error("Invalid Dir2d format: " + Value);
}

std::optional<UniChar> tryParseUniChar(const std::string &Value) {
  const char CharDelim = '\'';
  if (Value.size() < 3 || Value.size() > 6 || Value[0] != CharDelim ||
      Value[Value.size() - 1] != CharDelim) {
    return {};
  }
  unsigned char Data[4] = {0};

  // Allow escaped for '\''
  std::size_t Offset = Value[1] == '\\' ? 2 : 1;
  for (std::size_t Idx = Offset; Idx < Value.size() - 1; Idx++) {
    Data[Idx - Offset] = Value[Idx];
  }
  return UniChar(Data);
}

UniChar parseUniChar(const std::string &Value) {
  if (auto Char = tryParseUniChar(Value)) {
    return *Char;
  }
  throw std::runtime_error("Invalid unicode char format: " + Value);
}

RgbColor parseRgbColor(const std::string &Value) {
  std::regex Regex("\"?#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})\"?");
  std::smatch Match;
  if (!std::regex_match(Value, Match, Regex)) {
    throw std::runtime_error("Invalid RGB color format: " + Value);
  }
  RgbColor Color;
  Color.R = std::stoul(Match[1].str(), nullptr, 16);
  Color.G = std::stoul(Match[2].str(), nullptr, 16);
  Color.B = std::stoul(Match[3].str(), nullptr, 16);
  return Color;
}

ColoredUniChar parseColoredUniChar(const std::string &Value) {
  if (auto Char = tryParseUniChar(Value)) {
    return *Char;
  }
  static const std::regex Regex("\\{('.*'), (\"#[0-9a-fA-F]+\")\\}");
  std::smatch Match;
  if (std::regex_match(Value, Match, Regex)) {
    auto UC = parseUniChar(Match[1]);
    auto Color = parseRgbColor(Match[2]);
    return ColoredUniChar{UC, Color};
  }
  throw std::runtime_error("Invalid colored unicode char format: " + Value);
}

} // namespace ymir::Config