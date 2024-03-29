#ifndef YMIR_CONFIG_TYPES_HPP
#define YMIR_CONFIG_TYPES_HPP

#include <cstdlib>
#include <regex>
#include <ymir/Config/Parser.hpp>
#include <ymir/Terminal.hpp>
#include <ymir/TypeHelpers.hpp>
#include <ymir/Types.hpp>

namespace ymir::Config {

template <typename U> void registerYmirTypes() {
  registerAnyDumper<Size2d<U>>(
      [](Size2d<U> Size, std::ostream &Out) { Out << Size; });
  registerAnyDumper<Point2d<U>>(
      [](Point2d<U> Point, std::ostream &Out) { Out << Point; });
  registerAnyDumper<Rect2d<U>>(
      [](Rect2d<U> Rect, std::ostream &Out) { Out << Rect; });
  registerAnyDumper<Dir2d>([](Dir2d Dir, std::ostream &Out) { Out << Dir; });
}

template <typename U> Size2d<U> parseSize2d(const std::string &Value) {
  static const std::regex Regex("(Size2d)?\\{\\s*([0-9]+),\\s*([0-9]+)\\s*\\}");
  std::smatch Match;
  if (std::regex_match(Value, Match, Regex)) {
    return Size2d<U>{std::stoi(Match[2]), std::stoi(Match[3])};
  }
  throw std::runtime_error("Invalid SIze2d format: " + Value);
}

template <typename U> Point2d<U> parsePoint2d(const std::string &Value) {
  static const std::regex Regex(
      "(Point2d)?\\{\\s*((-)?[0-9]+),\\s*((-)?[0-9]+)\\s*\\}");
  std::smatch Match;
  if (std::regex_match(Value, Match, Regex)) {
    return Point2d<U>{std::stoi(Match[2]), std::stoi(Match[4])};
  }
  throw std::runtime_error("Invalid Point2d format: " + Value);
}

template <typename U> Rect2d<U> parseRect2d(const std::string &Value) {
  static const std::regex Regex("(Rect2d)?\\{(.*\\}), (.*)\\}");
  std::smatch Match;
  if (std::regex_match(Value, Match, Regex)) {
    return Rect2d<U>{parsePoint2d<U>(Match[2]), parseSize2d<U>(Match[3])};
  }
  throw std::runtime_error("Invalid Rect2d format: " + Value);
}

Dir2d parseDir2d(const std::string &Value);

std::optional<UniChar> tryParseUniChar(const std::string &Value);

UniChar parseUniChar(const std::string &Value);

RgbColor parseRgbColor(const std::string &Value);

ColoredUniChar parseColoredUniChar(const std::string &Value);

template <typename U> void registerYmirTypes(Parser &P) {
  P.registerType("Size2d", parseSize2d<U>);
  P.registerType("Point2d", parsePoint2d<U>);
  P.registerType("Rect2d", parseRect2d<U>);
  P.registerType("Dir2d", parseDir2d);
  P.registerType("UniChar", parseUniChar);
  P.registerType("UC", parseUniChar);
  P.registerType("ColoredUniChar", parseColoredUniChar);
  P.registerType("C", parseColoredUniChar);

  registerYmirTypes<U>();
}

} // namespace ymir::Config

#endif // #ifndef YMIR_CONFIG_TYPES_HPP