#include <gtest/gtest.h>

TEST(MOV_TEST, temp) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
  EXPECT_EQ(0, 1);
}