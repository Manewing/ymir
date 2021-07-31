#include <iostream>
#include <ymir/Config/AnyDict.hpp>
#include <ymir/Config/TypeHelpers.hpp>

namespace ymir::Config {

void AnyDict::set(const std::string &Key, std::any Value) {
  Map[Key] = std::move(Value);
}

AnyDict AnyDict::getSubDict(const std::string &Prefix) const {
  AnyDict SubDict;
  for (auto const &[Key, Value] : *this) {
    if (Key.rfind(Prefix, 0) == 0) {
      auto SubKey = Key.substr(Prefix.size(), Key.size() - Prefix.size());
      SubDict[SubKey] = Value;
    }
  }
  return SubDict;
}
const std::any &AnyDict::getAny(const std::string &Key, const std::any &Value,
                       const std::type_info &Type) const {
  return getAny(Key, Value, &Type);
}

const std::any &AnyDict::getAny(const std::string &Key, const std::any &Value,
                                const std::type_info *Type) const {
  if (Type != nullptr && Value.type().hash_code() != Type->hash_code()) {
    throw std::runtime_error("Could not cast configuration value '" + Key +
                             "' which is of type '" +
                             getTypeIdName(Value.type()) + "' to type '" +
                             getTypeIdName(*Type) + "'");
  }
  return Value;
}

const std::any &AnyDict::getInternal(const std::string &Key,
                                     const std::type_info &Type) const {
  return getInternal(Key, &Type);
}

const std::any &AnyDict::getInternal(const std::string &Key,
                                     const std::type_info *Type) const {
  auto It = Map.find(Key);
  if (It == Map.end()) {
    throw std::runtime_error("No configuration value '" + Key + "' present");
  }
  return getAny(Key, It->second, Type);
}

std::ostream &operator<<(std::ostream &Out, const AnyDict &Dict) {
  Out << "{" << std::endl;
  for (auto const &[Key, Value] : Dict) {
    Out << "  \"" << Key << "\": ";
    dumpAny(Out, Value) << std::endl;
  }
  Out << "}" << std::endl;
  return Out;
}

} // namespace ymir::Config