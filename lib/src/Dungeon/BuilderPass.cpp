#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/BuilderPass.hpp>
#include <ymir/TypeHelpers.hpp>

namespace ymir::Dungeon {

void BuilderPass::setBuilderAlias(const std::string &Builder,
                                  const std::string &Alias) {
  if (!BuilderFactory.count(Builder)) {
    throw std::runtime_error("Can not add alias '" + Alias +
                             "' for unregistered builder type '" + Builder +
                             "'");
  }
  BuilderAlias[Alias] = Builder;
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
    getInternal(BuilderName);
  }
  InitCtxPtr = nullptr;
}

void BuilderPass::run(BuilderContext &Ctx) {
  for (auto const &BuilderName : Sequence) {
    try {
      Builders.at(BuilderName)->run(*this, Ctx);
    } catch (const std::exception &E) {
      throw std::runtime_error("While running " + BuilderName + ": " +
                               E.what());
    }
  }
} // namespace ymir::Dungeon

BuilderBase &BuilderPass::getInternal(const std::string &BuilderName) {

  // Check if there is an alias registered for the name
  std::string BuilderType = BuilderName;
  if (auto It = BuilderAlias.find(BuilderName); It != BuilderAlias.end()) {
    BuilderType = It->second;
  }

  // Get the builder from the builders or create it if it does not yet exist
  std::shared_ptr<BuilderBase> BuilderPtr;
  if (auto It = Builders.find(BuilderName); It != Builders.end()) {
    BuilderPtr = It->second;
  } else if (auto It = BuilderFactory.find(BuilderType);
             It != BuilderFactory.end()) {
    BuilderPtr = It->second(BuilderName);
    Builders[BuilderName] = BuilderPtr;
  } else {
    throw std::runtime_error("No builder of type '" + BuilderType +
                             "' with name '" + BuilderName +
                             "' registered at pass");
  }

  // Check whether the builder has been initialized
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