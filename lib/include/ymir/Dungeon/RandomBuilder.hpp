#ifndef YMIR_RANDOM_BUILDER_HPP
#define YMIR_RANDOM_BUILDER_HPP

#include <functional>
#include <ymir/Dungeon/BuilderBase.hpp>

namespace ymir::Dungeon {

template <typename RandomEngineType> class RandomBuilder : public BuilderBase {
public:
  using BuilderBase::BuilderBase;

  void init(BuilderPass &Pass, BuilderContext &C) override {
    BuilderBase::init(Pass, C);
    bool SaltWithName = getCfgOr<bool>("use_salt", "dungeon/use_salt", true);
    unsigned Seed = getCfgOr<unsigned>("seed", "dungeon/seed", 0);
    if (SaltWithName) {
      std::hash<std::string> NameHasher;
      Seed = NameHasher(getName()) ^ Seed;
    }
    RndEng.seed(Seed);
  }

protected:
  RandomEngineType RndEng;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_RANDOM_BUILDER_HPP
