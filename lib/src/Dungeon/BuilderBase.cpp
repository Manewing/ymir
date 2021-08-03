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

Config::AnyDict BuilderBase::getCfg() const {
  return getPass().cfg().getSubDict(getName() + "/");
}

} // namespace ymir::Dungeon