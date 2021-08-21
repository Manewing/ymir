#include <gtest/gtest.h>
#include <ymir/Config/Types.hpp>

using namespace ymir;
using namespace ymir::Config;

namespace {

[[maybe_unused]] void dumpHex(const char Chars[5]) {
  std::cerr << "{" << std::hex;
  const char *Sep = "";
  for (unsigned Idx = 0; Idx < 5; Idx++) {
    std::cerr << Sep << "0x" << (static_cast<int>(Chars[Idx]) & 0xff);
    Sep = ", ";
  }
  std::cerr << std::dec << "}";
}

TEST(ConfigTypesTest, ParseUniChar) {
  EXPECT_EQ(parseUniChar("'a'"), UniChar('a'));
  EXPECT_EQ(parseUniChar("'τ'"), UniChar({0xcf, 0x84, 0x00, 0x00}));
  EXPECT_THROW(parseUniChar("'a"), std::runtime_error);
}

TEST(ConfigTypesTest, ParseRgbColor) {
  EXPECT_EQ(parseRgbColor("\"#123456\""), (RgbColor{0x12, 0x34, 0x56}));
  EXPECT_THROW(parseRgbColor("\"123456\""), std::runtime_error);
  EXPECT_THROW(parseRgbColor("\"#56\""), std::runtime_error);
}

TEST(ConfigTypesTest, ParseColoredUniChar) {
  EXPECT_EQ(parseColoredUniChar("'a'"), ColoredUniChar{'a'});
  EXPECT_EQ(parseColoredUniChar("{'a', \"#00ff00\"}"),
            ColoredUniChar('a', RgbColor{0x00, 0xff, 0x00}));
  EXPECT_EQ(parseColoredUniChar("{'τ', \"#11ff33\"}"),
            ColoredUniChar(UniChar({0xcf, 0x84, 0x00, 0x00}),
                           RgbColor{0x11, 0xff, 0x33}));
}

} // namespace