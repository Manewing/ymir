#ifndef YMIR_CONFIG_PARSER_HPP
#define YMIR_CONFIG_PARSER_HPP

#include <filesystem>
#include <functional>
#include <map>
#include <regex>
#include <string>
#include <ymir/Config/AnyDict.hpp>

namespace ymir::Config {

class Parser {
public:
  static const std::regex HeaderRe;
  static const std::regex AssignRe;
  static const std::regex ListItemRe;

  static char parseChar(const std::string &Value);
  static std::string parseString(const std::string &Value);
  static unsigned parseUnsigned(const std::string &Value);
  static float parseFloat(const std::string &Value);
  static int parseSigned(const std::string &Value);
  static bool parseBool(const std::string &Value);
  static std::any parseAnyType(const std::string &Value);

public:
  using ParserCallbackType = std::function<std::any(const std::string &)>;

public:
  Parser();

  inline AnyDict &getCfg() { return Configuration; }
  inline const AnyDict &getCfg() const { return Configuration; }

  void parse(const std::filesystem::path &Path);
  void parse(const std::string &Str);
  void parse(const char *Str);

  void registerType(const std::string &TypeName,
                    ParserCallbackType ParserCallback);

  void set(const std::string &Header, const std::string &Name, std::any Value);

  template <typename T> T &get(const std::string &Path) {
    return Configuration.get<T>(Path);
  }

  template <typename T>
  T &get(const std::string &Header, const std::string &Name) {
    return get<T>(Header + "/" + Name);
  }

  template <typename T> auto asList(const std::string &Name) {
    return Configuration.asList<T>(Name + "/");
  }

private:
  void parseInternal(std::istream &In);
  void parseLine(std::string Line);
  bool parseAssignment(const std::string& Line);
  bool parseListItem(const std::string &Line);
  const ParserCallbackType &getTypeParser(const std::string &Type);

private:
  std::string CurrentHeader;
  std::map<std::string, ParserCallbackType> ParserCallbacks;
  AnyDict Configuration;
  std::vector<std::any>* CurrentList = nullptr;
};

} // namespace ymir::Config

#endif // #ifndef YMIR_CONFIG_PARSER_HPP