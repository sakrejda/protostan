#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
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
TEST(binaryProtoStreamWriter,roundTripKeyDouble) {
  std::string original_key = "KEY";
  double original_value = 3.14159;
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripKeyDouble.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripKeyDouble.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_key, original_value);
  delete writer;

  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(original_key, pb.stan_parameter_output().key());
  EXPECT_EQ(original_value, pb.stan_parameter_output().value());
}

TEST(binaryProtoStreamWriter,roundTripKeyInteger) {
  std::string original_key = "KEY";
  int original_value = 42;
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripKeyInteger.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripKeyInteger.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_key, original_value);
  delete writer;

  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(original_key, pb.stan_integer_output().key());
  EXPECT_EQ(original_value, pb.stan_integer_output().value());
}

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

  EXPECT_EQ(original_key, pb.stan_string_output().key());
  EXPECT_EQ(original_value, pb.stan_string_output().value());
}

TEST(binaryProtoStreamWriter,roundTripKeyDoubleInt) {
  std::string original_key = "KEY";
  double original_value[5] = {3.1, 6.2, 12.4, 99.311111111111, -1000.12} ;
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripKeyDoubleInt.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripKeyDoubleInt.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_key, original_value, 5);
  delete writer;

  for ( uint i=0; i < 5; ++i ) {
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ(original_key, pb.stan_parameter_output().key());
    EXPECT_EQ(i, pb.stan_parameter_output().indexing(0));
    EXPECT_EQ(original_value[i], pb.stan_parameter_output().value());
  }
  delete pb_istream;
  close(fd);
}

TEST(binaryProtoStreamWriter,roundTripKeyDoubleIntInt) {
  std::string original_key = "KEY";
  double original_value[6] = {3.1, 6.2, 12.4, 99.311111111111, -1000.12, 666.0} ;
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripKeyDoubleIntInt.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripKeyDoubleIntInt.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_key, original_value, 3, 2);
  delete writer;

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 2; ++j) {
      success = stan::proto::read_delimited_pb(&pb, pb_istream);
      EXPECT_EQ(true, success);
      EXPECT_EQ(original_key, pb.stan_parameter_output().key());
      EXPECT_EQ(i, pb.stan_parameter_output().indexing(0));
      EXPECT_EQ(j, pb.stan_parameter_output().indexing(1));
      EXPECT_EQ(original_value[i * 2 + j], pb.stan_parameter_output().value());
    }
  }
  delete pb_istream;
  close(fd);
}

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

TEST(binaryProtoStreamWriter,roundTripVectorString) {
  std::vector<std::string> original_value;
  uint n_strings = 100000;
  for ( int i=0; i < n_strings; ++i ) {
    original_value.push_back(gen_random(10));
  }
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripVectorString.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripVectorString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_value);
  delete writer;

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

TEST(binaryProtoStreamWriter,roundTripVectorDouble) {
  std::random_device random;
  std::mt19937 engine(random());
  std::uniform_real_distribution<> U(0, 1);
  std::vector<double> original_value;
  uint n_doubles = 100000;
  for ( int i=0; i < n_doubles; ++i ) {
    original_value.push_back(U(engine));
  }
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripVectorDouble.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripVectorDouble.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_value);
  delete writer;

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

TEST(binaryProtoStreamWriter,roundTripString) {
  std::string original_value = "I AM YOUR STRING";
  std::ofstream* ofs = new std::ofstream("/tmp/test-roundTripString.pb", std::ios::trunc | std::ios::out | std::ios::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer* writer = new stan::interface_callbacks::writer::binary_proto_stream_writer(ofs);
  stan::proto::StanMessage pb;
  int fd;
  fd = open("/tmp/test-roundTripString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;

  (*writer)(original_value);
  delete writer;

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

