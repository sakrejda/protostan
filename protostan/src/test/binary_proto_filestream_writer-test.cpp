#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/callbacks/binary_proto_stream_writer.hpp>
#include <protostan/util/rw_delimited_pb.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <random>

// This test instantiates writer/ofs explicitely as pointers to control
// their lifetime s.t. the buffer internal to the writer is flushed
// correctly prior to attempting a read and so that the writer can flush
// the ofstream buffer correctly.  This is not an efficient way to write
// to file as there are two layers of buffering.  The correct way to do
// it is to do a class ("binary_proto_file_writer") that uses a
// FileOutputStream on the inside as it's ZeroCopyOutputStream type.
// This is just to have a test.
//


std::string gen_random(const int len) {
  std::string s;
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < len; ++i) {
    s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  return s;
}

TEST(binaryProtoFileWriter,roundTripVectorString) {
  std::vector<std::string> original_value;
  uint n_strings = 100000;
  for ( int i=0; i < n_strings; ++i ) {
    original_value.push_back(gen_random(10));
  }

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripVectorString.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::callbacks::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::callbacks::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_value);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripVectorString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
  for ( uint i=0; i < n_strings; ++i ) {
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ("name", pb.stan_string_output().key());
    EXPECT_EQ(i, pb.stan_string_output().indexing(0));
    EXPECT_EQ(original_value[i], pb.stan_string_output().value());
  }
  delete pb_istream;
  close(fd);
}

TEST(binaryProtoFileWriter,roundTripVectorDouble) {
  std::random_device random;
  std::mt19937 engine(random());
  std::uniform_real_distribution<> U(0, 1);
  std::vector<double> original_value;
  uint n_doubles = 100000;
  for ( int i=0; i < n_doubles; ++i ) {
    original_value.push_back(U(engine));
  }

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripVectorDouble.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::callbacks::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::callbacks::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_value);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripVectorDouble.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
  for ( uint i=0; i < n_doubles; ++i ) {
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ("value", pb.stan_parameter_output().key());
    EXPECT_EQ(i, pb.stan_parameter_output().indexing(0));
    EXPECT_EQ(original_value[i], pb.stan_parameter_output().value());
  }
  delete pb_istream;
  close(fd);
}

TEST(binaryProtoFileWriter,roundTripString) {
  std::string original_value = "I AM YOUR STRING";

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripString.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::callbacks::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::callbacks::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_value);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(true, success);
  EXPECT_EQ("message", pb.stan_string_output().key());
  EXPECT_EQ(original_value, pb.stan_string_output().value());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}

