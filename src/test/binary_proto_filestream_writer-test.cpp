#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
#include <protostan/util/rw_delimited_pb.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <random>

/*
 * These tests instantiate the streams using pointer/new and the writer
 * using pointer/new in order to control object lifetime and stream
 * flushing.  This is not the right way to use them in other code where
 * a stream is flushed on destruction and read by a separate process.
 * In general the writer should be instantiated as (probably with a
 * typedef in here somewhere):
 *
 * stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb> writer(ostream);
 *
 * where ostream is a ZeroCopyOutputStream (or subclass), here a
 * FileOuptutStream constructed from a file descriptor.  The
 * ZeroCopyOutputStream will be deleted by the writer destructor. The
 * file descriptor must be closed independently of the writer's destruction.
 */
TEST(binaryProtoFileWriter,roundTripKeyDouble) {
  std::string original_key = "KEY";
  double original_value = 3.14159;

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripKeyDouble.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripKeyDouble.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(original_key, pb.stan_parameter_output().key());
  EXPECT_EQ(original_value, pb.stan_parameter_output().value());
}


TEST(binaryProtoFileWriter,roundTripKeyInteger) {
  std::string original_key = "KEY";
  int original_value = 42;

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripKeyInteger.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripKeyInteger.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(original_key, pb.stan_integer_output().key());
  EXPECT_EQ(original_value, pb.stan_integer_output().value());
}

TEST(binaryProtoFileWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripKeyString.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripKeyString.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;
  close(fd);

  EXPECT_EQ(original_key, pb.stan_string_output().key());
  EXPECT_EQ(original_value, pb.stan_string_output().value());
}

TEST(binaryProtoFileWriter,roundTripKeyDoubleInt) {
  std::string original_key = "KEY";
  double original_value[5] = {3.1, 6.2, 12.4, 99.311111111111, -1000.12} ;

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripKeyDoubleInt.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value, 5);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripKeyDoubleInt.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
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

TEST(binaryProtoFileWriter,roundTripKeyDoubleIntInt) {
  std::string original_key = "KEY";
  double original_value[6] = {3.1, 6.2, 12.4, 99.311111111111, -1000.12, 666.0} ;

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripKeyDoubleIntInt.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value, 3, 2);
  delete writer;
  close(fd);

  stan::proto::StanMessage pb;
  fd = open("/tmp/test-roundTripKeyDoubleIntInt.pb", O_RDONLY); 
  google::protobuf::io::FileInputStream* pb_istream = new google::protobuf::io::FileInputStream(fd);
  bool success;
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

TEST(binaryProtoFileWriter,roundTripVectorString) {
  std::vector<std::string> original_value;
  uint n_strings = 100000;
  for ( int i=0; i < n_strings; ++i ) {
    original_value.push_back(gen_random(10));
  }

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-roundTripVectorString.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

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
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

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
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

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

