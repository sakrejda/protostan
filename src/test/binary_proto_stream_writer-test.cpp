#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>


TEST(binaryProtoWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
  std::stringstream store;
  stan::interface_callbacks::writer::binary_proto_stream_writer writer(store);
  stan::proto::StanStringOutput pb;
  google::protobuf::io::IstreamInputStream read_stream(&store);

  writer(original_key, original_value);
  read_delimited_pb(&pb, &read_stream);
  EXPECT_EQ(pb.key(),original_key);
  EXPECT_EQ(pb.value(),original_value);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}
