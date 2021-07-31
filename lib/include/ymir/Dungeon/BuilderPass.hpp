#ifndef YMIR_DUNGEON_BUILDER_PASS_H
#define YMIR_DUNGEON_BUILDER_PASS_H

#include <any>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ymir/Config/AnyDict.hpp>
#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/TypeHelpers.hpp>

namespace ymir::Dungeon {

class BuilderPass {
public:
  template <typename BuilderType> void registerBuilder() {
    Builders[BuilderType::Name] = std::make_shared<BuilderType>();
  }

  void setBuilderAlias(const std::string &Builder, const std::string &Alias);

  void configure(Config::AnyDict Configuration);

  void setSequence(std::vector<std::string> Sequence);

  void init(BuilderContext &Ctx);

  void run(BuilderContext &Ctx);

  template <typename T> T &get(const std::string &BuilderName) {
    return dynamic_cast<T &>(getInternal(BuilderName));
  }

  template <typename T, typename = decltype(T::Name)> T &get() {
    return get<T>(T::Name);
  }

  Config::AnyDict &cfg() { return Configuration; }

  const Config::AnyDict &cfg() const { return Configuration; }

private:
  BuilderBase &getInternal(const std::string &BuilderName);

private:
  BuilderContext *InitCtxPtr = nullptr;
  std::vector<std::string> Sequence;
  std::map<std::string, std::shared_ptr<BuilderBase>> Builders;
  Config::AnyDict Configuration;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_BUILDER_PASS_H