#include <algorithm>
#include <ymir/Config/String.hpp>

namespace ymir::Config {

std::string &transform_ltrim(std::string &Str) {
  Str.erase(Str.begin(), std::find_if(Str.begin(), Str.end(), [](char Char) {
              return !std::isspace(Char);
            }));
  return Str;
}

std::string ltrim(const std::string &Str) {
  std::string StrCopy = Str;
  transform_ltrim(StrCopy);
  return StrCopy;
}

std::string &transform_rtrim(std::string &Str) {
  Str.erase(std::find_if(Str.rbegin(), Str.rend(),
                         [](char Char) { return !std::isspace(Char); })
                .base(),
            Str.end());
  return Str;
}

std::string rtrim(const std::string &Str) {
  std::string StrCopy = Str;
  transform_rtrim(StrCopy);
  return StrCopy;
}

std::string &transform_trim(std::string &Str) {
  return transform_ltrim(transform_rtrim(Str));
}

std::string trim(const std::string &Str) {
  std::string StrCopy = Str;
  transform_trim(StrCopy);
  return StrCopy;
}

std::string &transform_lower(std::string &Str) {
  std::transform(Str.begin(), Str.end(), Str.begin(),
                 [](auto Char) { return std::tolower(Char); });
  return Str;
}

std::string lower(const std::string &Str) {
  std::string StrCopy = Str;
  transform_lower(StrCopy);
  return StrCopy;
}

} // namespace ymir::Config