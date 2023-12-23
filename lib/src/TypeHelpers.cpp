#include <iostream>
#include <ymir/TypeHelpers.hpp>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace ymir {

std::string getTypeIdName(const std::type_info &TypeInfo) {
  int Status;
  std::string TypeName = TypeInfo.name();
  #ifdef __GNUG__
  char *Demangled = abi::__cxa_demangle(TypeName.c_str(), NULL, NULL, &Status);
  if (Status == 0) {
    TypeName = Demangled;
    std::free(Demangled);
  }
  #endif
  return TypeName;
}

namespace internal {
template <typename T>
static auto
toAnyDumper(const internal::TypeVisitor<T, std::ostream &> &Dumper) {
  return internal::toAnyVisitorIndex<T, std::ostream &>(Dumper);
}

std::map<std::type_index, AnyVisitor<std::ostream &>> &getAnyDumpers() {
  static std::map<std::type_index, AnyVisitor<std::ostream &>> AnyDumpers = {
      toAnyDumper<int>([](int Value, std::ostream &Out) { Out << Value; }),
      toAnyDumper<unsigned>(
          [](unsigned Value, std::ostream &Out) { Out << Value << "U"; }),
      toAnyDumper<float>(
          [](float Value, std::ostream &Out) { Out << std::fixed << Value; }),
      toAnyDumper<double>(
          [](double Value, std::ostream &Out) { Out << std::fixed << Value; }),
      toAnyDumper<char>(
          [](char Value, std::ostream &Out) { Out << "'" << Value << "'"; }),
      toAnyDumper<std::string>([](const std::string &Value, std::ostream &Out) {
        Out << "\"" << Value << "\"";
      }),
  };
  return AnyDumpers;
}
} // namespace internal

std::ostream &dumpAny(std::ostream &Out, const std::any &Any) {
  const auto It = internal::getAnyDumpers().find(std::type_index(Any.type()));
  if (It == internal::getAnyDumpers().end()) {
    Out << "<unhandled: " << getTypeIdName(Any.type()) << ">";
    return Out;
  }
  It->second(Any, Out);
  return Out;
}

} // namespace ymir