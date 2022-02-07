#include <gtest/gtest.h>
#include <ymir/Config/Parser.hpp>

using namespace ymir;

namespace {

TEST(ConfigParserTest, Bool) {
  Config::Parser Parser;
  Parser.parse("[hdr]\nval_true = true\nval_false = false");
  EXPECT_TRUE(Parser.get<bool>("hdr", "val_true"));
  EXPECT_FALSE(Parser.get<bool>("hdr", "val_false"));
}

TEST(ConfigParserTest, Int) {
  Config::Parser Parser;
  Parser.parse("[hdr]\nval = 42");
  EXPECT_EQ(Parser.get<int>("hdr", "val"), 42);
}

TEST(ConfigParserTest, String) {
  Config::Parser Parser;
  Parser.parse("[hdr]\nval = \"test\"");
  EXPECT_EQ(Parser.get<std::string>("hdr", "val"), "test");
}

TEST(ConfigParserTest, Char) {
  Config::Parser Parser;
  Parser.parse("[hdr]\nval = 'x'\nval_2 = '\\''");
  EXPECT_EQ(Parser.get<char>("hdr", "val"), 'x');
  EXPECT_EQ(Parser.get<char>("hdr", "val_2"), '\'');
}

} // namespace
