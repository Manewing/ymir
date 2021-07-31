#ifndef YMIR_CONFIG_ANY_DICT_HPP
#define YMIR_CONFIG_ANY_DICT_HPP

#include <any>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace ymir::Config {

class AnyDict {
public:
  using MapType = std::map<std::string, std::any>;

public:
  void set(const std::string &Key, std::any Value);

  inline auto size() const { return Map.size(); }
  inline bool empty() const { return Map.empty(); }

  inline std::any &operator[](const std::string &Key) { return Map[Key]; }

  inline const std::any &at(const std::string &Key) const {
    return Map.at(Key);
  }

  template <typename T> inline T &get(const std::string &Key) {
    return *std::any_cast<T>(
        const_cast<std::any *>(&getInternal(Key, typeid(T))));
  }

  template <typename T> inline const T &get(const std::string &Key) const {
    return *std::any_cast<T>(&getInternal(Key, typeid(T)));
  }

  AnyDict getSubDict(const std::string &Prefix) const;

  template <typename T> std::vector<std::pair<std::string, T>> toVec() const {
    std::vector<std::pair<std::string, T>> Result;
    Result.reserve(Map.size());
    for (auto const &[Key, Value] : Map) {
      Result.emplace_back(Key,
                          *std::any_cast<T>(&getAny(Key, Value, typeid(T))));
    }
    return Result;
  }

  template <typename T> std::map<std::string, T> toMap() const {
    std::map<std::string, T> Result;
    for (auto const &[Key, Value] : Map) {
      Result[Key] = *std::any_cast<T>(&getAny(Key, Value, typeid(T)));
    }
    return Result;
  }

  inline auto begin() { return Map.begin(); }
  inline auto begin() const { return Map.begin(); }
  inline auto end() { return Map.end(); }
  inline auto end() const { return Map.end(); }

private:
  const std::any &getAny(const std::string &Key, const std::any &Value,
                         const std::type_info &Type) const;
  const std::any &getAny(const std::string &Key, const std::any &Value,
                         const std::type_info *Type = nullptr) const;
  const std::any &getInternal(const std::string &Key,
                              const std::type_info &Type) const;
  const std::any &getInternal(const std::string &Key,
                              const std::type_info *Type = nullptr) const;

private:
  MapType Map;
};

std::ostream &operator<<(std::ostream &Out, const AnyDict &Dict);

} // namespace ymir::Config

#endif // #ifndef YMIR_CONFIG_ANY_DICT_HPP