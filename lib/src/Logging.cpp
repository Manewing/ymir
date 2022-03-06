#include <ctime>
#include <iomanip>
#include <iostream>
#include <ymir/Logging.hpp>

namespace ymir {

LogMessageAssembler::LogMessageAssembler(Logger &Log, int Level)
    : Log(Log), Level(Level) {}

LogMessageAssembler::~LogMessageAssembler() {
  if (Log.getLogLevel() < Level) {
    return;
  }
  Log.logMessage(Level, PartialMessage.str());
}

DefaultLogFormatter::DefaultLogFormatter(bool HasTimeStamp)
    : HasTimeStamp(HasTimeStamp) {
  LevelNames = {
      {Logger::ErrorLevel, "error"},
      {Logger::WarningLevel, "warning"},
      {Logger::InfoLevel, "info"},
      {Logger::DebugLevel, "debug"},
  };
}

std::string
DefaultLogFormatter::formatMessage(int Level, const std::string &Name,
                                   const std::string &Message) const {
  std::stringstream SS;

  if (HasTimeStamp) {
    std::time_t Time = std::time(nullptr);
    std::tm TM = *std::localtime(&Time);
    SS << "[" << std::put_time(&TM, "%F %T") << "] ";
  }

  if (!Name.empty()) {
    SS << Name << ":";
  }

  auto It = LevelNames.find(Level);
  if (It != LevelNames.end()) {
    SS << It->second << ": ";
  } else {
    SS << "level{" << Level << "}: ";
  }

  // Add log message content
  SS << Message << "\n";

  return SS.str();
}

LogHandler::LogHandler() {
  Formatter = std::make_shared<DefaultLogFormatter>();
}

void LogHandler::setFormatter(std::shared_ptr<const LogFormatter> LF) {
  Formatter = std::move(LF);
}

StreamLogHandler::StreamLogHandler(std::ostream *Out) : Out(Out) {}

void StreamLogHandler::setStream(std::ostream *O) { Out = O; }

void StreamLogHandler::logMessage(int Level, const std::string &Name,
                                  const std::string &Message) {
  if (!Out || !Formatter) {
    return;
  }
  *Out << Formatter->formatMessage(Level, Name, Message);
}

StdoutLogHandler::StdoutLogHandler() : StreamLogHandler(&std::cout) {}

StderrLogHandler::StderrLogHandler() : StreamLogHandler(&std::cerr) {}

StringStreamLogHandler::StringStreamLogHandler()
    : StreamLogHandler(nullptr), SS() {
  setStream(&SS);
}

std::string StringStreamLogHandler::string() const { return SS.str(); }

FileLogHandler::FileLogHandler(const std::filesystem::path &LogFile,
                               bool Append)
    : StreamLogHandler(nullptr) {
  auto Mode = Append ? (std::ios::out | std::ios::app) : std::ios::out;
  FileStream.open(LogFile.c_str(), Mode);
  if (FileStream.is_open()) {
    setStream(&FileStream);
  } else {
    throw std::runtime_error("FileLogHandler: could not open log file: " +
                             LogFile.string());
  }
}

void FileLogHandler::logMessage(int Level, const std::string &Name,
                                const std::string &Message) {
  StreamLogHandler::logMessage(Level, Name, Message);
  // FIXME only every Nth log message or time interval
  FileStream.flush();
}

Logger::Logger(std::string N) : Name(std::move(N)) {}

void Logger::addHandler(std::shared_ptr<LogHandler> Handler) {
  Handlers.push_back(std::move(Handler));
}

LogMessageAssembler Logger::log(int Level) {
  return LogMessageAssembler(*this, Level);
}

LogMessageAssembler Logger::error() { return log(ErrorLevel); }

LogMessageAssembler Logger::warning() { return log(WarningLevel); }

LogMessageAssembler Logger::info() { return log(InfoLevel); }

LogMessageAssembler Logger::debug() { return log(DebugLevel); }

void Logger::logMessage(int Level, const std::string &Message) {
  for (auto &Handler : Handlers) {
    Handler->logMessage(Level, Name, Message);
  }
}

} // namespace ymir