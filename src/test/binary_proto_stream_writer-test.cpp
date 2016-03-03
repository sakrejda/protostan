#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>

// This test instantiates writer/ofs explicitely as pointers to control
// their lifetime s.t. the buffer internal to the writer is flushed
// correctly prior to attempting a read and so that the writer can flush
// the ofstream buffer correctly.  This is not an efficient way to write
// to file as there are two layers of buffering.  The correct way to do
// it is to do a class ("binary_proto_file_writer") that uses a
// FileOutputStream on the inside as it's ZeroCopyOutputStream type.
// This is just to have a test.
TEST(binaryProtoStreamWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripKeyString.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripKeyString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_key, original_value);
  delete writer;

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


