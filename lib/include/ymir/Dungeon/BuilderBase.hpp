#ifndef YMIR_DUNGEON_BUILDER_BASE_HPP
#define YMIR_DUNGEON_BUILDER_BASE_HPP

#include <optional>
#include <ymir/Config/AnyDict.hpp>
#include <ymir/Dungeon/BuilderPass.hpp>
#include <ymir/Logging.hpp>

namespace ymir::Dungeon {

class BuilderContext {
public:
  virtual ~BuilderContext() = default;

  template <typename T> T &get() { return dynamic_cast<T &>(*this); }

  template <typename T> T *get_or_null() { return dynamic_cast<T *>(this); }

public:
  Logger *Log = nullptr;
};

class BuilderBase {
public:
  explicit BuilderBase(std::string Name);
  virtual ~BuilderBase() = default;

  virtual const char *getType() const = 0;
  const std::string &getName() const;

  bool isInit() const;

  virtual void init(BuilderPass &Pass, BuilderContext &Ctx);
  virtual void run(BuilderPass &Pass, BuilderContext &Ctx);

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

  template <typename T = BuilderPass> const T &getPass() const {
    if (CurrentPassPtr == nullptr) {
      throw std::runtime_error("Access to uninialized builder pass");
    }
    return dynamic_cast<T &>(*CurrentPassPtr);
  }

  template <typename T> const T &getCfg(const std::string &Key) const {
    auto Path = getName() + "/" + Key;
    return getPass().cfg().template get<T>(Path);
  }

  template <typename T>
  const T &getCfgOr(const std::string &Key, const T &Default) const {
    auto Path = getName() + "/" + Key;
    if (!getPass().cfg().count(Path)) {
      return Default;
    }
    return getPass().cfg().template get<T>(Path);
  }

  template <typename T>
  std::optional<T> getCfgOpt(const std::string &Key) const {
    auto Path = getName() + "/" + Key;
    if (!getPass().cfg().count(Path)) {
      return std::nullopt;
    }
    return getPass().cfg().template get<T>(Path);
  }

  template <typename T>
  const T &getCfg(const std::string &Key,
                  const std::string &FallbackKey) const {
    auto Path = getName() + "/" + Key;
    if (getPass().cfg().count(Path)) {
      return getPass().cfg().template get<T>(Path);
    }
    return getPass().cfg().template get<T>(FallbackKey);
  }

  template <typename T>
  const T &getCfgOr(const std::string &Key, const std::string &FallbackKey,
                    const T &Default) const {
    auto Path = getName() + "/" + Key;
    if (!getPass().cfg().count(Path) && !getPass().cfg().count(FallbackKey)) {
      return Default;
    }
    return getCfg<T>(Key, FallbackKey);
  }

  Config::AnyDict getSubCfg(const std::string &Key) const;
  Config::AnyDict getSubCfg(const std::string &Key,
                            const std::string &FallbackKey) const;

private:
  std::string Name;
  BuilderPass *CurrentPassPtr = nullptr;
  BuilderContext *CurrentCtxPtr = nullptr;
};

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_BUILDER_BASE_HPP