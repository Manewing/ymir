#include <ymir/Dungeon/BuilderPass.hpp>

namespace ymir::Dungeon {

void BuilderPass::setBuilderAlias(const std::string &Builder,
                                  const std::string &Alias) {
  if (Builders.count(Alias)) {
    throw std::out_of_range("Duplicate TODO");
  }
  Builders[Alias] = Builders.at(Builder);
}

void BuilderPass::configure(Config::AnyDict Configuration) {
  this->Configuration = std::move(Configuration);
}

void BuilderPass::setSequence(std::vector<std::string> Sequence) {
  this->Sequence = std::move(Sequence);
}

void BuilderPass::init(BuilderContext &Ctx) {
  InitCtxPtr = &Ctx;
  for (auto const &BuilderName : Sequence) {
    Builders.at(BuilderName)->init(*this, Ctx);
  }
  InitCtxPtr = nullptr;
}

void BuilderPass::run(BuilderContext &Ctx) {
  for (auto const &BuilderName : Sequence) {
    Builders.at(BuilderName)->run(*this, Ctx);
  }
}

BuilderBase &BuilderPass::getInternal(const std::string &BuilderName) {
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
  return *BuilderPtr;
}

} // namespace ymir::Dungeon