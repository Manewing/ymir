#ifndef YMIR_DUNGEON_BUILDER_BASE_H
#define YMIR_DUNGEON_BUILDER_BASE_H

namespace ymir::Dungeon {
class BuilderPass;
}

namespace ymir::Dungeon {

class BuilderContext {
public:
  virtual ~BuilderContext() = default;

  template <typename T>
  T &get() { return dynamic_cast<T&>(*this); }

  template <typename T>
  T *get_or_null() { return dynamic_cast<T*>(this); }
};

class BuilderBase {
public:
  virtual ~BuilderBase() = default;

  virtual const char *getName() const = 0;
  virtual void init(BuilderPass &Pass, BuilderContext &Ctx) {
    (void)Pass;
    (void)Ctx;
  }
  virtual void run(BuilderPass &Pass, BuilderContext &Ctx) = 0;
};

}

#endif // #ifndef YMIR_DUNGEON_BUILDER_BASE_H