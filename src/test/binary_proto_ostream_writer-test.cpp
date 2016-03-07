#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
#include <protostan/util/rw_delimited_pb.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <random>

// This test instantiates writer/ios explicitely as pointers to control
// their lifetime s.t. the buffer internal to the writer is flushed
// correctly prior to attempting a read and so that the writer can flush
// the iostream buffer correctly.  This is not an efficient way to write
// to file as there are two layers of buffering.  The correct way to do
// it is to do a class ("binary_proto_file_writer") that uses a
// FileOutputStream on the inside as it's ZeroCopyOutputStream type.
// This is just to have a test.
//
TEST(binaryProtoOstreamWriter,roundTripKeyDouble) {
  std::string original_key = "KEY";
  double original_value = 3.14159;

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);
  (*writer)(original_key, original_value);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;

  EXPECT_EQ(original_key, pb.stan_parameter_output().key());
  EXPECT_EQ(original_value, pb.stan_parameter_output().value());
}

TEST(binaryProtoOstreamWriter,roundTripKeyInteger) {
  std::string original_key = "KEY";
  int original_value = 42;

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;

  EXPECT_EQ(original_key, pb.stan_integer_output().key());
  EXPECT_EQ(original_value, pb.stan_integer_output().value());
}

TEST(binaryProtoOstreamWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  EXPECT_EQ(true, success);
  delete pb_istream;

  EXPECT_EQ(original_key, pb.stan_string_output().key());
  EXPECT_EQ(original_value, pb.stan_string_output().value());
}

TEST(binaryProtoOstreamWriter,roundTripKeyDoubleInt) {
  std::string original_key = "KEY";
  double original_value[5] = {3.1, 6.2, 12.4, 99.311111111111, -1000.12} ;

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value, 5);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  for ( uint i=0; i < 5; ++i ) {
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ(original_key, pb.stan_parameter_output().key());
    EXPECT_EQ(i, pb.stan_parameter_output().indexing(0));
    EXPECT_EQ(original_value[i], pb.stan_parameter_output().value());
  }
  delete pb_istream;

}

TEST(binaryProtoOstreamWriter,roundTripKeyDoubleIntInt) {
  std::string original_key = "KEY";
  double original_value[6] = {3.1, 6.2, 12.4, 99.311111111111, -1000.12, 666.0} ;

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_key, original_value, 3, 2);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
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

TEST(binaryProtoOstreamWriter,roundTripVectorString) {
  std::vector<std::string> original_value;
  uint n_strings = 100000;
  for ( int i=0; i < n_strings; ++i ) {
    original_value.push_back(gen_random(10));
  }

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_value);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  for ( uint i=0; i < n_strings; ++i ) {
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ("name", pb.stan_string_output().key());
    EXPECT_EQ(i, pb.stan_string_output().indexing(0));
    EXPECT_EQ(original_value[i], pb.stan_string_output().value());
  }
  delete pb_istream;

}

TEST(binaryProtoOstreamWriter,roundTripVectorDouble) {
  std::random_device random;
  std::mt19937 engine(random());
  std::uniform_real_distribution<> U(0, 1);
  std::vector<double> original_value;
  uint n_doubles = 100000;
  for ( int i=0; i < n_doubles; ++i ) {
    original_value.push_back(U(engine));
  }

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_value);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  for ( uint i=0; i < n_doubles; ++i ) {
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ("value", pb.stan_parameter_output().key());
    EXPECT_EQ(i, pb.stan_parameter_output().indexing(0));
    EXPECT_EQ(original_value[i], pb.stan_parameter_output().value());
  }
  delete pb_istream;

}

TEST(binaryProtoOstreamWriter,roundTripString) {
  std::string original_value = "I AM YOUR STRING";

  std::stringstream* ios = new std::stringstream();
  google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
  stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>* 
    writer = new stan::interface_callbacks::writer::binary_proto_stream_writer<stan::proto::write_delimited_pb>(pb_ostream);

  (*writer)(original_value);
  delete writer;

  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
  bool success;
  success = stan::proto::read_delimited_pb(&pb, pb_istream);
  delete pb_istream;

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

