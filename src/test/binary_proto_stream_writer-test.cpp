#include "gtest/gtest.h"
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>

bool read_delimited_pb(
    google::protobuf::io::ZeroCopyInputStream* rawInput,
    google::protobuf::MessageLite* message) {
    google::protobuf::io::CodedInputStream input(rawInput);

  // Read the size.
  uint32_t size;
  if (!input.ReadVarint32(&size)) return false;

  // Tell the stream not to read beyond that size.
  google::protobuf::io::CodedInputStream::Limit limit =
      input.PushLimit(size);

  // Parse the message.
  if (!message->MergeFromCodedStream(&input)) return false;
  if (!input.ConsumedEntireMessage()) return false;

  // Release the limit.
  input.PopLimit(limit);

  return true;
}

TEST(binaryProtoWriter,roundTripKeyString) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
  std::stringstream store;
  stan::interface_callbacks::writer::binary_proto_stream_writer writer(store);
  google::protobuf::MessageLite pb;
  google::protobuf::io::IstreamInputStream read_stream(store);

  writer(original_key, original_value);
  pb = read_delimited_pb(read_stream);
  EXPECT_EQ(pb->key(),original_key);
  EXPECT_EQ(pb->value(),original_value);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}
