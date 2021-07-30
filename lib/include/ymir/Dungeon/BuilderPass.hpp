#ifndef YMIR_DUNGEON_BUILDER_PASS_H
#define YMIR_DUNGEON_BUILDER_PASS_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ymir/Dungeon/BuilderBase.hpp>

namespace ymir::Dungeon {

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

  void setSequence(std::vector<std::string> Sequence) {
    this->Sequence = std::move(Sequence);
  }

  void init(BuilderContext &Ctx) {
    for (auto &[Name, Builder] : Builders) {
      (void)Name;
      Builder->init(*this, Ctx);
    }
  }

  void run(BuilderContext &Ctx) {
    for (auto const &BuilderName : Sequence) {
      Builders.at(BuilderName)->run(*this, Ctx);
    }
  }

  template <typename T> T &get(const std::string &BuilderName) {
    return dynamic_cast<T &>(*Builders.at(BuilderName));
  }

  template <typename T, typename = decltype(T::Name)> T &get() {
    return dynamic_cast<T &>(*Builders.at(T::Name));
  }

private:
  std::vector<std::string> Sequence;
  std::map<std::string, std::shared_ptr<BuilderBase>> Builders;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_BUILDER_PASS_H