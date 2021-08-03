#ifndef YMIR_DUNGEON_BUILDER_PASS_H
#define YMIR_DUNGEON_BUILDER_PASS_H

#include <any>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <functional>
#include <ymir/Config/AnyDict.hpp>

namespace ymir::Dungeon {
class BuilderBase;
class BuilderContext;
}

namespace ymir::Dungeon {

class BuilderPass {
public:
  using BuilderCreator =
      std::function<std::shared_ptr<BuilderBase>(std::string)>;

public:
  template <typename BuilderType> void registerBuilder() {
    BuilderFactory[BuilderType::Type] = [](std::string Name) {
      return std::make_shared<BuilderType>(std::move(Name));
    };
  }

  void setBuilderAlias(const std::string &Builder, const std::string &Alias);

  void configure(Config::AnyDict Configuration);

  void setSequence(std::vector<std::string> Sequence);

  void init(BuilderContext &Ctx);

  void run(BuilderContext &Ctx);

  template <typename T> T &get(const std::string &BuilderName) {
    return dynamic_cast<T &>(getInternal(BuilderName));
  }

  template <typename T, typename = decltype(T::Type)> T &get() {
    return get<T>(T::Type);
  }

  Config::AnyDict &cfg() { return Configuration; }

  const Config::AnyDict &cfg() const { return Configuration; }

private:
  BuilderBase &getInternal(const std::string &BuilderName);

private:
  BuilderContext *InitCtxPtr = nullptr;
  std::vector<std::string> Sequence;
  // FIXME unique_ptr?
  std::map<std::string, std::shared_ptr<BuilderBase>> Builders;
  std::map<std::string, BuilderCreator> BuilderFactory;
  std::map<std::string, std::string> BuilderAlias;
  Config::AnyDict Configuration;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_BUILDER_PASS_H