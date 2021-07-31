#ifndef YMIR_TYPE_HELPERS_HPP
#define YMIR_TYPE_HELPERS_HPP

#include <any>
#include <functional>
#include <iosfwd>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace ymir {

std::string getTypeIdName(const std::type_info &TypeInfo);

template <typename T> std::string getTypeName() {
  return getTypeIdName(typeid(T));
}

namespace internal {

template <typename... Args>
using AnyVisitor = std::function<void(const std::any &, Args...)>;

template <typename T, typename... Args>
using TypeVisitor = std::function<void(const T &, Args...)>;

template <typename T, typename... Args>
AnyVisitor<Args...> toAnyVisitor(const TypeVisitor<T, Args...> &VisitFunc) {
  return [Func = VisitFunc](std::any const &Any, Args... Arg) {
    if constexpr (std::is_void_v<T>) {
      Func(Arg...);
    } else {
      Func(std::any_cast<T const &>(Any), Arg...);
    }
  };
}

template <typename T, typename... Args>
std::pair<std::type_index, AnyVisitor<Args...>>
toAnyVisitorIndex(const TypeVisitor<T, Args...> &VisitFunc) {
  return {std::type_index(typeid(T)), toAnyVisitor(VisitFunc)};
}

std::map<std::type_index, AnyVisitor<std::ostream &>> &getAnyDumpers();

} // namespace internal

std::ostream &dumpAny(std::ostream &Out, const std::any &Any);

template <typename T>
void registerAnyDumper(const internal::TypeVisitor<T, std::ostream &> &Dumper) {
  internal::getAnyDumpers().insert(
      internal::toAnyVisitorIndex<T, std::ostream &>(Dumper));
}

} // namespace ymir

#endif // #ifndef YMIR_CONFIG_TYPE_HELPERS_HPP