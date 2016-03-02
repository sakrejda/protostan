#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>

TEST(binaryProtoWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripKeyString.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer writer(*ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripKeyString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  writer(original_key, original_value);
  delete ofs;

  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(original_key, original_key);
  EXPECT_EQ(original_key, pb.stan_string_output().key());
  EXPECT_EQ(original_value, pb.stan_string_output().value());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}
