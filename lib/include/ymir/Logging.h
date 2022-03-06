#ifndef YMIR_LOGGING_HPP
#define YMIR_LOGGING_HPP

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
namespace ymir {
class Logger;
}

namespace ymir {

/// Temporary object that handles assembling a log message via operator<<
/// which is logged in the destructor
class LogMessageAssembler {
public:
  LogMessageAssembler(Logger &Log, int Level);
  ~LogMessageAssembler();

  template <typename Type> LogMessageAssembler &operator<<(const Type &T);

private:
  Logger &Log;
  int Level;
  std::stringstream PartialMessage;
};

/// Interface definition for log message formatters. Handles formatting
/// the log message before outputting.
class LogFormatter {
public:
  virtual ~LogFormatter() = default;
  virtual std::string formatMessage(int Level, const std::string &LogName,
                                    const std::string &Message) const = 0;
};

/// Default log message formatter, formats message in the form:
///   <log_name>:<level_name>: <message>
/// or if timestamp is enabled:
///   [<timestamp>] <log_name>:<level_name>: <message>
class DefaultLogFormatter : public LogFormatter {
public:
  DefaultLogFormatter(bool HasTimeStamp = true);

  std::string formatMessage(int Level, const std::string &LogName,
                            const std::string &Message) const final;

private:
  bool HasTimeStamp;
  std::unordered_map<int, const char *> LevelNames;
};

/// Interface class for handling logging of a message, e.g. writing the log
/// message to stdout. Each handler can be equipped with a custom formatter.
class LogHandler {
public:
  LogHandler();
  virtual ~LogHandler() = default;
  void setFormatter(std::shared_ptr<const LogFormatter> Formatter);

  virtual void logMessage(int Level, const std::string &LogName,
                          const std::string &Message) = 0;

protected:
  std::shared_ptr<const LogFormatter> Formatter;
};

/// Base class for log handlers that use a stream for writing the log message.
class StreamLogHandler : public LogHandler {
public:
  StreamLogHandler(std::ostream *Out = nullptr);
  void setStream(std::ostream *Out = nullptr);

  void logMessage(int Level, const std::string &LogName,
                  const std::string &Message) override;

private:
  std::ostream *Out = nullptr;
};

/// Log handler that writes to stdout
class StdoutLogHandler : public StreamLogHandler {
public:
  StdoutLogHandler();
};

/// Log handler that writes to stderr
class StderrLogHandler : public StreamLogHandler {
public:
  StderrLogHandler();
};

/// Log handler that writes to a stringstream
class StringStreamLogHandler : public StreamLogHandler {
public:
  StringStreamLogHandler();

  /// Returns string of logged messages
  std::string string() const;

private:
  std::stringstream SS;
};

/// Log handler that writes to a specific file
class FileLogHandler : public StreamLogHandler {
public:
  FileLogHandler(const std::filesystem::path &LogFile, bool Append = false);

  void logMessage(int Level, const std::string &LogName,
                  const std::string &Message) override;

private:
  std::ofstream FileStream;
};

/// Logger class that can be equipped with log handlers.
class Logger {
public:
  friend class LogMessageAssembler;

  /// Default log levels
  static constexpr int ErrorLevel = 0;
  static constexpr int WarningLevel = 10;
  static constexpr int InfoLevel = 20;
  static constexpr int DebugLevel = 30;

public:
  Logger(std::string Name);
  virtual ~Logger() = default;

  void addHandler(std::shared_ptr<LogHandler> Handler);

  LogMessageAssembler log(int Level);
  LogMessageAssembler error();
  LogMessageAssembler warning();
  LogMessageAssembler info();
  LogMessageAssembler debug();

  int getLogLevel() const { return LogLevel; }
  void setLogLevel(int Level) { LogLevel = Level; }

protected:
  virtual void logMessage(int Level, const std::string &Message);

private:
  int LogLevel = 30;
  std::string Name;
  std::vector<std::shared_ptr<LogHandler>> Handlers;
};

template <typename Type>
LogMessageAssembler &LogMessageAssembler::operator<<(const Type &T) {
  if (Log.getLogLevel() < Level) {
    return *this;
  }
  PartialMessage << T;
  return *this;
}

} // namespace ymir

#endif // #ifndef YMIR_LOGGING_HPP
