#include "gtest/gtest.h"
#include <stan/proto/stanc.pb.h>
#include <protostan/lang/compiler.hpp>

TEST(stanc, minimumModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;

  request.set_model_name("test");
  request.set_model_code("model {}");
  request.set_model_file_name("test.stan");
  response = stan::proto::compile(request);
  EXPECT_EQ(stan::proto::StanCompileResponse::SUCCESS, response.state());
  EXPECT_STREQ("", response.messages().c_str());
  // "int main(" should *not* be in the program C++ code
  EXPECT_EQ(std::string::npos, response.cpp_code().find("int main("));
  // "class test" should be, since the model name is "test"
  EXPECT_NE(std::string::npos, response.cpp_code().find("class test"));
}


TEST(stanc, invalidModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;

  request.set_model_name("test");
  request.set_model_code("invalid model code");
  request.set_model_file_name("test.stan");
  response = stan::proto::compile(request);
  EXPECT_EQ(stan::proto::StanCompileResponse::ERROR, response.state());
  EXPECT_STREQ("", response.cpp_code().c_str());
  EXPECT_STRNE("", response.messages().c_str());
  EXPECT_NE(std::string::npos, response.messages().find("PARSER EXPECTED: <model declaration"));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}



