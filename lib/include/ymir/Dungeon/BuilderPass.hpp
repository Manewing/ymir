#ifndef YMIR_DUNGEON_BUILDER_PASS_H
#define YMIR_DUNGEON_BUILDER_PASS_H

#include <any>
#include <cxxabi.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ymir/Dungeon/BuilderBase.hpp>

namespace ymir::Dungeon {

template <typename T> std::string getTypeIdName(const T &TypeId) {
  int Status;
  std::string TypeName = TypeId.name();
  char *Demangled = abi::__cxa_demangle(TypeName.c_str(), NULL, NULL, &Status);
  if (Status == 0) {
    TypeName = Demangled;
    std::free(Demangled);
  }
  return TypeName;
}

// TOOD move
template <typename T> std::string getTypeName() {
  return getTypeIdName(typeid(T));
}

class BuilderPass {
public:
  template <typename BuilderType> void registerBuilder() {
    Builders[BuilderType::Name] = std::make_shared<BuilderType>();
  }

  void setBuilderAlias(const std::string &Builder, const std::string &Alias) {
    if (Builders.count(Alias)) {
      throw std::out_of_range("Duplicate TODO");
    }
    Builders[Alias] = Builders.at(Builder);
  }

  void configure(std::map<std::string, std::any> Configuration) {
    this->Configuration = std::move(Configuration);
  }

  void setSequence(std::vector<std::string> Sequence) {
    this->Sequence = std::move(Sequence);
  }

  void init(BuilderContext &Ctx) {
    InitCtxPtr = &Ctx;
    for (auto const &BuilderName : Sequence) {
      Builders.at(BuilderName)->init(*this, Ctx);
    }
    InitCtxPtr = nullptr;
  }

  void run(BuilderContext &Ctx) {
    for (auto const &BuilderName : Sequence) {
      Builders.at(BuilderName)->run(*this, Ctx);
    }
  }

  template <typename T> T &get(const std::string &BuilderName) {
    // TODO move to internal function
    auto It = Builders.find(BuilderName);
    if (It == Builders.end()) {
      throw std::runtime_error("No builder with name '" + BuilderName +
                               "' registered at pass");
    }
    auto &BuilderPtr = It->second;
    if (!BuilderPtr->isInit()) {
      if (InitCtxPtr == nullptr) {
        throw std::runtime_error("Uninitalized builder '" + BuilderName +
                                 "'requested after initialization");
      }
      BuilderPtr->init(*this, *InitCtxPtr);
    }
    return dynamic_cast<T &>(*BuilderPtr);
  }

  template <typename T, typename = decltype(T::Name)> T &get() {
    return get<T>(T::Name);
  }

  template <typename T> T &getConfigValue(const std::string &Name) {
    auto It = Configuration.find(Name);
    if (It == Configuration.end()) {
      throw std::runtime_error("No configuration value '" + Name + "' present");
    }
    auto *Value = std::any_cast<T>(&It->second);
    if (Value == nullptr) {
      throw std::runtime_error("Could not cast configuration value '" + Name +
                               "' which is of type '" +
                               getTypeIdName(It->second.type()) +
                               "' to type '" + getTypeName<T>() + "'");
    }
    return *Value;
  }

private:
  BuilderContext *InitCtxPtr = nullptr;
  std::vector<std::string> Sequence;
  std::map<std::string, std::shared_ptr<BuilderBase>> Builders;
  std::map<std::string, std::any> Configuration;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_BUILDER_PASS_H