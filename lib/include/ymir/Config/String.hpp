#ifndef YMIR_CONFIG_STRING_HPP
#define YMIR_CONFIG_STRING_HPP

#include <string>

namespace ymir::Config {

std::string &ltrim(std::string &Str);

std::string &rtrim(std::string &Str);

std::string &trim(std::string &Str);

} // namespace ymir::Config

#endif // #ifndef YMIR_CONFIG_STRING_HPP