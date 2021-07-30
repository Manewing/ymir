#ifndef YMIR_DUNGEON_BUILDER_BASE_H
#define YMIR_DUNGEON_BUILDER_BASE_H

namespace ymir::Dungeon {
class BuilderPass;
}

namespace ymir::Dungeon {

class BuilderContext {
public:
  virtual ~BuilderContext() = default;

  template <typename T> T &get() { return dynamic_cast<T &>(*this); }

  template <typename T> T *get_or_null() { return dynamic_cast<T *>(this); }
};

class BuilderBase {
public:
  virtual ~BuilderBase() = default;

  virtual const char *getName() const = 0;

  virtual void init(BuilderPass &Pass, BuilderContext &Ctx) {
    CurrentPassPtr = &Pass;
    CurrentCtxPtr = &Ctx;
  }

  virtual void run(BuilderPass &Pass, BuilderContext &Ctx) {
    if (CurrentPassPtr != &Pass) {
      throw std::runtime_error("CurrentPassPtr != &Pass"); // TODO
    }
    if (CurrentCtxPtr != &Ctx) {
      throw std::runtime_error("CurrentCtxPtr != &Ctx"); // TODO
    }
  }

protected:
  template <typename T> T &getCtx() {
    if (CurrentCtxPtr == nullptr) {
      throw std::runtime_error("Access to uninialized builder context");
    }
    return CurrentCtxPtr->get<T>();
  }

  template <typename T = BuilderPass> T &getPass() {
    if (CurrentPassPtr == nullptr) {
      throw std::runtime_error("Access to uninialized builder pass");
    }
    return dynamic_cast<T &>(*CurrentPassPtr);
  }

private:
  BuilderPass *CurrentPassPtr = nullptr;
  BuilderContext *CurrentCtxPtr = nullptr;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_BUILDER_BASE_H