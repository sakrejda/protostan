#include "gtest/gtest.h"
#include <protostan/lang/compiler.hpp>

TEST(stanc, minimumModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;

  request.set_model_name("test");
  request.set_model_code("model {}");
  request.set_model_file_name("test.stan");
  response = stan::proto::compile(request);
  // FIXME: These tests are lame, but that's all that
  // stan-dev/stan does... (?)
  EXPECT_EQ(2, response.state());
  EXPECT_EQ("", response.messages());
  EXPECT_EQ(std::string::npos, response.cpp_code().find("int main("));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}



