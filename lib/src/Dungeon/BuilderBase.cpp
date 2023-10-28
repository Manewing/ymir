#include <iostream>
#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/BuilderPass.hpp>

namespace ymir::Dungeon {

BuilderBase::BuilderBase(std::string Name) : Name(std::move(Name)) {}

const std::string &BuilderBase::getName() const { return Name; }

bool BuilderBase::isInit() const {
  return CurrentPassPtr != nullptr && CurrentCtxPtr != nullptr;
}

void BuilderBase::init(BuilderPass &Pass, BuilderContext &Ctx) {
  CurrentPassPtr = &Pass;
  CurrentCtxPtr = &Ctx;
}

void BuilderBase::run(BuilderPass &Pass, BuilderContext &Ctx) {
  if (CurrentPassPtr != &Pass) {
    throw std::runtime_error("CurrentPassPtr != &Pass"); // TODO
  }
  if (CurrentCtxPtr != &Ctx) {
    throw std::runtime_error("CurrentCtxPtr != &Ctx"); // TODO
  }
}

Config::AnyDict BuilderBase::getSubCfg(const std::string &Key) const {
  auto Path = getName() + "/" + Key;
  return getPass().cfg().getSubDict(Path);
}

Config::AnyDict BuilderBase::getSubCfg(const std::string &Key,
                                       const std::string &FallbackKey) const {
  auto Path = getName() + "/" + Key;
  if (getPass().cfg().count(Path)) {
    return getPass().cfg().getSubDict(Path);
  }
  return getPass().cfg().getSubDict(FallbackKey);
}

} // namespace ymir::Dungeon