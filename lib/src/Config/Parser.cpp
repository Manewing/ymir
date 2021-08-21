#include <fstream>
#include <ymir/Config/Parser.hpp>
#include <ymir/Config/String.hpp>

namespace ymir::Config {

const std::regex Parser::HeaderRe =
    std::regex(R"~(\s*\[([a-z/A-Z_0-9]+)\]\s*)~");
const std::regex Parser::AssignRe =
    std::regex(R"~(([a-zA-Z_0-9]+)(:\s*([a-zA-Z_0-9]+))?\s*=\s*(.+))~");

char Parser::parseChar(const std::string &Value) {
  const char CharDelim = '\'';
  if (Value.size() != 3 || Value[0] != CharDelim ||
      Value[Value.size() - 1] != CharDelim) {
    throw std::runtime_error("Invalid char format: " + Value +
                             " only ASCII allowed");
  }
  return Value[1];
}

std::string Parser::parseString(const std::string &Value) {
  const char StringDelim = '"';
  if (Value.size() < 2 || Value[0] != StringDelim ||
      Value[Value.size() - 1] != StringDelim) {
    return Value;
  }
  return Value.substr(1, Value.size() - 2);
}

unsigned Parser::parseUnsigned(const std::string &Value) {
  if (!Value.empty() && Value[Value.size() - 1] == 'U') {
    unsigned Val = std::stoul(Value.substr(0, Value.size() - 1).c_str());
    return Val;
  }
  if (Value.empty()) {
    return 0;
  }
  unsigned Val = std::stoul(Value.c_str());
  return Val;
}

float Parser::parseFloat(const std::string &Value) {
  float Val = std::stof(Value);
  return Val;
}

int Parser::parseSigned(const std::string &Value) {
  int Val = std::stol(Value);
  return Val;
}

bool Parser::parseBool(const std::string &Value) {
  std::string ValueCopy = lower(Value);
  return ValueCopy == "true";
}

namespace {
std::optional<bool> tryParseBool(const std::string &Value) {
  std::string ValueCopy = lower(Value);
  if (Value == "true" || Value == "false") {
    return Value == "true";
  }
  return std::nullopt;
}
} // namespace

std::any Parser::parseAnyType(const std::string &Value) {
  if (Value.empty()) {
    return std::any();
  }
  switch (Value[0]) {
  case '\'':
    return parseChar(Value);
  case '"':
    return parseString(Value);
  default:
    break;
  }
  if (auto BoolOpt = tryParseBool(Value)) {
    return *BoolOpt;
  }
  if (std::any_of(Value.begin(), Value.end(), [](char Char) {
        return !std::isdigit(Char) && Char != '-' && Char != '.' &&
               Char != '+' && Char != 'U';
      })) {
    return parseString(Value);
  }
  if (Value[Value.size() - 1] == 'U') {
    return parseUnsigned(Value);
  }
  if (Value.find('.') != std::string::npos) {
    return parseFloat(Value);
  }
  return parseSigned(Value);
}

Parser::Parser() {
  registerType("", parseAnyType);
  registerType("char", parseChar);
  registerType("string", parseString);
  registerType("float", parseFloat);
  registerType("int", parseSigned);
  registerType("unsigned", parseUnsigned);
  registerType("bool", parseBool);
}

void Parser::parse(const std::filesystem::path &Path) {
  std::ifstream In(Path.c_str(), std::ios::in);
  if (!In.is_open()) {
    throw std::runtime_error("Could not open " + Path.string() +
                             " for reading");
  }
  parseInternal(In);
}

void Parser::parse(const std::string &Str) {
  std::stringstream SS(Str);
  parseInternal(SS);
}

void Parser::parse(const char *Str) {
  std::stringstream SS(Str);
  parseInternal(SS);
}

void Parser::registerType(const std::string &TypeName,
                          ParserCallbackType ParserCallback) {
  ParserCallbacks[TypeName] = ParserCallback;
}

void Parser::set(const std::string &Header, const std::string &Name,
                 std::any Value) {
  if (Header.empty()) {
    throw std::runtime_error("Empty header for " + Name);
  }
  std::string Path = Header + "/" + Name;
  Configuration.set(Path, std::move(Value));
}

void Parser::parseInternal(std::istream &In) {
  std::string Line;
  while (std::getline(In, Line)) {
    parseLine(Line);
  }
}

void Parser::parseLine(std::string Line) {
  transform_trim(Line);
  if (Line.empty() || Line[0] == '#') {
    return;
  }

  std::smatch HeaderMatch;
  if (std::regex_match(Line, HeaderMatch, HeaderRe)) {
    CurrentHeader = HeaderMatch[1];
    return;
  }
  std::smatch AssignMatch;
  if (std::regex_match(Line, AssignMatch, AssignRe)) {
    const std::string Name = AssignMatch[1];
    const std::string Type = AssignMatch[3];
    const std::string Value = AssignMatch[4];
    try {
      set(CurrentHeader, Name, getTypeParser(Type)(Value));
    } catch (std::exception &E) {
      throw std::runtime_error("While parsing line '" + Line +
                               "': " + E.what());
    }
    return;
  }

  throw std::runtime_error("Syntax error for line: " + Line);
}

const Parser::ParserCallbackType &
Parser::getTypeParser(const std::string &Type) {
  auto It = ParserCallbacks.find(Type);
  if (It == ParserCallbacks.end()) {
    throw std::runtime_error("No parser registered for type: '" + Type + "'");
  }
  return It->second;
}

} // namespace ymir::Config