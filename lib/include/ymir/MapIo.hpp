#ifndef YMIR_MAP_IO_HPP
#define YMIR_MAP_IO_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <ymir/Map.hpp>

// TODO move operator<< for map here?
namespace ymir {

ymir::Map<char, int> loadMap(const std::vector<std::string> &MapLines);
ymir::Map<char, int> loadMap(const std::filesystem::path &Path);

} // namespace ymir

#endif // #ifndef YMIR_MAP_IO_HPP