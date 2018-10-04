#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/callbacks/binary_proto_stream_writer.hpp>
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

TEST(binaryProtoOstreamWriter,roundTripDebug) {
  std::string original_value;
  int id = 64;
  uint n_strings = 100000;
  for ( int i=0; i < n_strings; ++i ) {
    original_value = gen_random(10);
    std::stringstream* ios = new std::stringstream();
    google::protobuf::io::OstreamOutputStream* pb_ostream = new google::protobuf::io::OstreamOutputStream(ios);
    stan::callbacks::binary_proto_stream_logger<stan::proto::write_delimited_pb>* 
    writer = new stan::callbacks::binary_proto_stream_logger<stan::proto::write_delimited_pb>(pb_ostream, id);

    (*writer).debug(original_value);
    delete writer;

    stan::proto::StanMessage pb;
    google::protobuf::io::IstreamInputStream* pb_istream = new google::protobuf::io::IstreamInputStream(ios);
    bool success;
    success = stan::proto::read_delimited_pb(&pb, pb_istream);
    EXPECT_EQ(true, success);
    EXPECT_EQ(stan::proto::StanMessage::Type::LOG, pb().stan_log_message().type());
    EXPECT_EQ(id, pb().stan_log_message().id());
    EXPECT_EQ(stan::proto::StanLog::Type::STRING, pb().stan_log_message().type());
    EXPECT_EQ(stan::proto::StanLog::Level::DEBUG, pb().stan_log_message().level());
    EXPECT_EQ(original_value, pb().stan_log_message().log_message());
  }
  delete pb_istream;
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}

