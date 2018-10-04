#include <iostream>
#include "gtest/gtest.h"
#include <stan/proto/compile.pb.h>
#include <protostan/lang/compiler.hpp>

TEST(compile, minimumModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;

  request.set_model_name("test");
  request.set_model_code("model {}");
  response = stan::proto::compile(request);
  EXPECT_EQ(stan::proto::StanCompileResponse::SUCCESS, response.state());
  EXPECT_STREQ("", response.messages().c_str());
  // "int main(" should *not* be in the program C++ code
  EXPECT_EQ(std::string::npos, response.cpp_code().find("int main("));
  // "class test" should be, since the model name is "test"
  EXPECT_NE(std::string::npos, response.cpp_code().find("class test"));
}

TEST(compile, invalidModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;

  request.set_model_name("test");
  request.set_model_code("invalid model code");
  response = stan::proto::compile(request);
  EXPECT_EQ(stan::proto::StanCompileResponse::ERROR, response.state());
  EXPECT_STREQ("", response.cpp_code().c_str());
  EXPECT_STRNE("", response.messages().c_str());
  std::cout << response.messages() << std::endl;
  EXPECT_NE(std::string::npos, response.messages().find("invalid model code"));
}

TEST(compile, noSuchDistributionModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;

  request.set_model_name("test");
  request.set_model_code("parameters {real z;} model {z ~ no_such_distribution();}");
  response = stan::proto::compile(request);
  EXPECT_EQ(stan::proto::StanCompileResponse::ERROR, response.state());
  EXPECT_STREQ("", response.cpp_code().c_str());
  EXPECT_STRNE("", response.messages().c_str());
  EXPECT_NE(std::string::npos, response.messages().find("Distribution no_such_distribution not found."));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}
