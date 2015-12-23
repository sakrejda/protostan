#include "gtest/gtest.h"
#include <protostan/lang/compiler.hpp>

TEST(stanc, minimumModelCompile) {
  stan::proto::StanCompileRequest request;
  stan::proto::StanCompileResponse response;
  
  request.set_model_name("test");
  request.set_model_code("model {}");
  request.set_model_file_name("test.stan");
  response = stan::proto::compile(request);
  std::cout << response.cpp_code() << std::endl;
  EXPECT_EQ(1,1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}



