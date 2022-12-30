#include <algorithm>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ymir/Logging.hpp>

namespace {

class MockLogger : public ymir::Logger {
public:
  using ymir::Logger::Logger;
  MOCK_METHOD(void, logMessage, (int, const std::string &));
};

std::string getFileContent(const std::filesystem::path &FilePath) {
  std::ifstream FileStream(FilePath.c_str());
  return std::string(std::istreambuf_iterator<char>(FileStream),
                     std::istreambuf_iterator<char>());
}

TEST(LoggingTest, LogMessageAssembler) {
  MockLogger Log("my_logger");
  EXPECT_CALL(Log, logMessage(ymir::Logger::ErrorLevel, "test: 42"));
  {
    ymir::LogMessageAssembler(Log, ymir::Logger::ErrorLevel) << "test: " << 42;
  }
}

TEST(LoggingTest, DefaultLogFormatter) {
  ymir::DefaultLogFormatter DLF(/*HasTimeStamp=*/false);
  EXPECT_EQ(DLF.formatMessage(ymir::Logger::ErrorLevel, "my_logger", "msg"),
            "my_logger:error: msg\n");
  EXPECT_EQ(DLF.formatMessage(42, "my_logger", "msg"),
            "my_logger:level{42}: msg\n");
  ymir::DefaultLogFormatter DLFT(/*HasTimeStamp=*/true);
  auto Msg = DLFT.formatMessage(-1, "my_logger", "msg");
  std::replace_if(
      Msg.begin(), Msg.begin() + 21,
      [](auto Char) { return std::isdigit(Char); }, '0');
  auto MsgRef = "[0000-00-00 00:00:00] my_logger:level{-1}: msg\n";
  EXPECT_EQ(Msg, MsgRef);
}

// TODO test StdoutLogHandler, StdcerrLogHandler
// TODO need stream capture to test std::cout, std::cerr output
TEST(LoggingTest, StringStreamLogHandler) {
  ymir::StringStreamLogHandler SSLH;
  SSLH.setFormatter(std::make_shared<ymir::DefaultLogFormatter>(false));
  SSLH.logMessage(-1, "my_logger", "msg_1");
  SSLH.logMessage(-2, "my_logger", "msg_2");
  EXPECT_EQ(SSLH.string(), "my_logger:level{-1}: msg_1\n"
                           "my_logger:level{-2}: msg_2\n");
}

TEST(LoggingTest, FileLogHandler) {
  auto DLF = std::make_shared<ymir::DefaultLogFormatter>(false);
  {
    ymir::FileLogHandler FLH("log.txt", /*Append=*/false);
    FLH.setFormatter(DLF);
    FLH.logMessage(-1, "my_logger", "msg_1");
    FLH.logMessage(-2, "my_logger", "msg_2");
    EXPECT_EQ(getFileContent("log.txt"), "my_logger:level{-1}: msg_1\n"
                                         "my_logger:level{-2}: msg_2\n");
  }
  {
    ymir::FileLogHandler FLH("log.txt", /*Append=*/true);
    FLH.setFormatter(DLF);
    FLH.logMessage(-3, "my_logger", "msg_3");
    EXPECT_EQ(getFileContent("log.txt"), "my_logger:level{-1}: msg_1\n"
                                         "my_logger:level{-2}: msg_2\n"
                                         "my_logger:level{-3}: msg_3\n");
  }
  {
    ymir::FileLogHandler FLH("log.txt", /*Append=*/false);
    FLH.setFormatter(DLF);
    FLH.logMessage(-4, "my_logger", "msg_4");
    EXPECT_EQ(getFileContent("log.txt"), "my_logger:level{-4}: msg_4\n");
  }
}

TEST(LoggingTest, FileLogHandlerLogFileCantBeOpened) {
  EXPECT_THROW(ymir::FileLogHandler FLH("path/that/does/not/exist/log.txt"),
               std::runtime_error);
}

TEST(LoggingTest, Logger) {
  ymir::Logger Log("my_logger");
  auto SSLH = std::make_shared<ymir::StringStreamLogHandler>();
  SSLH->setFormatter(std::make_shared<ymir::DefaultLogFormatter>(false));
  Log.addHandler(SSLH);

  Log.setLogLevel(ymir::Logger::InfoLevel);
  Log.error() << "test error";
  Log.warning() << "test warning";
  Log.info() << "test info";
  Log.debug() << "test debug";

  EXPECT_EQ(SSLH->string(), "my_logger:error: test error\n"
                            "my_logger:warning: test warning\n"
                            "my_logger:info: test info\n");
}

} // namespace