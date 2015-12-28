#include "gtest/gtest.h"

TEST(alwaysPass, pass) {
  EXPECT_EQ(1,1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}



