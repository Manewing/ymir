#include <ymir/Config/String.hpp>
#include <algorithm>

namespace ymir::Config {

std::string &ltrim(std::string &Str) {
  Str.erase(Str.begin(), std::find_if(Str.begin(), Str.end(), [](char Char) {
              return !std::isspace(Char);
            }));
  return Str;
}

std::string &rtrim(std::string &Str) {
  Str.erase(std::find_if(Str.rbegin(), Str.rend(),
                         [](char Char) { return !std::isspace(Char); })
                .base(),
            Str.end());
  return Str;
}

std::string &trim(std::string &Str) { return ltrim(rtrim(Str)); }

} // namespace ymir::Config