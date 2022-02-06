#include <gtest/gtest.h>
#include <ymir/Config/String.hpp>

using namespace ymir;

namespace {

TEST(StringTest, Trim) {
  EXPECT_EQ(Config::trim(""), "");
  EXPECT_EQ(Config::trim("   x x  "), "x x");
  EXPECT_EQ(Config::ltrim("   x x  "), "x x  ");
  EXPECT_EQ(Config::rtrim("   x x  "), "   x x");
}

TEST(StringTest, Lower) {
  EXPECT_EQ(Config::lower(""), "");
  EXPECT_EQ(Config::lower("AbCdEf"), "abcdef");
}

} // namespace