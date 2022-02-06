#ifndef YMIR_CONFIG_STRING_HPP
#define YMIR_CONFIG_STRING_HPP

#include <string>

namespace ymir::Config {

std::string &transform_ltrim(std::string &Str);
std::string ltrim(const std::string &Str);

std::string &transform_rtrim(std::string &Str);
std::string rtrim(const std::string &Str);

std::string &transform_trim(std::string &Str);
std::string trim(const std::string &Str);

std::string &transform_lower(std::string &Str);

std::string lower(const std::string &Str);

} // namespace ymir::Config

#endif // #ifndef YMIR_CONFIG_STRING_HPP