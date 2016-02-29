#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>


TEST(binaryProtoWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
  std::stringstream store(std::stringstream::binary);
  stan::interface_callbacks::writer::binary_proto_stream_writer writer(store);
  stan::proto::StanMessage pb;
  google::protobuf::io::IstreamInputStream read_stream(&store);
  bool success;

  std::cout << "BLAH: " << store.str() << std::endl;
  writer(original_key, original_value);
  store.seekg(0,store.beg);
  std::cout << "BLAH: " << store.str() << std::endl;

  success = stan::proto::read_delimited_pb(&pb, &read_stream);
  if (!success) throw("Read failed.");
  EXPECT_EQ(original_key, original_key);
  EXPECT_EQ(original_key, pb.stan_string_output().key());
  EXPECT_EQ(original_value, pb.stan_string_output().value());
//  pb.release_key();
//  pb.release_value();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}
