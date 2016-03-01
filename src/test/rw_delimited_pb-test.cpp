#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
#include <fcntl.h>


TEST(binaryProtoWriter, write_delimited_pb) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
//  std::stringstream store(std::stringstream::binary);
  int fd = open("/tmp/test-file.pb", O_CREAT | O_WRONLY | O_TRUNC);
  google::protobuf::io::ZeroCopyOutputStream* pb_ostream;
  google::protobuf::io::ZeroCopyInputStream* pb_istream;
  stan::proto::StanMessage pb, pb2;
  bool success;

  pb.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
  pb.mutable_stan_parameter_output()->set_key("THIS IS A KEY");
  pb.mutable_stan_parameter_output()->set_value(3.14159);

  pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  std::cout << "I WROTE: " << pb_ostream->ByteCount() << std::endl;
  success = stan::proto::write_delimited_pb(pb, pb_ostream);
  EXPECT_EQ(true, success);
  std::cout << "I WROTE: " << pb_ostream->ByteCount() << std::endl;
  delete pb_ostream;
  close(fd);

  fd = open("/tmp/test-file.pb", O_RDONLY); 
  pb_istream = new google::protobuf::io::FileInputStream(fd);
  std::cout << "I READ: " << pb_istream->ByteCount() << std::endl;
  success = stan::proto::read_delimited_pb(&pb2, pb_istream);
  EXPECT_EQ(true, success);
  std::cout << "I READ: " << pb_istream->ByteCount() << std::endl;
  delete pb_istream;
  close(fd);

  EXPECT_EQ(pb.stan_parameter_output().key(), pb2.stan_parameter_output().key());
  EXPECT_EQ(pb.stan_parameter_output().value(), pb2.stan_parameter_output().value());

}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  returnValue = RUN_ALL_TESTS();
  return returnValue;
}
