#include "gtest/gtest.h"
#include <stan/proto/stan-core.pb.h>
#include <stan/proto/sample.pb.h>
#include <protostan/interface_callbacks/writer/binary_proto_stream_writer.hpp>
#include <protostan/util/rw_delimited_pb.hpp>
#include <fcntl.h>

/*
 * This is a round-trip test which must control lifetime of the streams
 * and file descriptors so they are all instantiated as pointers.  The
 * stream is only flushed when deleted, and for good measure I close the
 * file after writing and re-open it for reading rather than messing
 * around with seekg/etc... a simple StanMessage is created, written to the
 * file, and then the data is read back into a separate StanMessage
 * before comaprison.
 *
 * In typical usage, the stream could be created
 * without the use of a pointer/new.  So something assuming two
 * StanMessage types are on hand (pb and pb2), something like:
 *
 * mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
 * int fd = open("/tmp/test-rwDelimitedPb.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
 * google::protobuf::io::FileOutputStream pb_ostream(fd);
 * success = stan::proto::write_delimited_pb(&pb, &pb_ostream);
 * 
 * The reading example below would be:
 *
 * fd = open("/tmp/test-rwDelimitedPb.pb", O_RDONLY); 
 * google::protobuf::io::FileInputStream pb_istream(fd);
 * success = stan::proto::read_delimited_pb(&pb2, &pb_istream);
 *
 */
TEST(binaryProtoWriter, rwDelimitedPb) {
  std::string original_key = "KEY";
  std::string original_value = "I AM YOUR STRING";
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open("/tmp/test-rwDelimitedPb.pb", O_CREAT | O_WRONLY | O_TRUNC, mode);
  google::protobuf::io::FileOutputStream* pb_ostream;
  google::protobuf::io::FileInputStream* pb_istream;
  stan::proto::StanMessage pb, pb2;
  bool success;

  pb.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
  pb.mutable_stan_parameter_output()->set_key("THIS IS A KEY");
  pb.mutable_stan_parameter_output()->set_value(3.14159);

  pb_ostream = new google::protobuf::io::FileOutputStream(fd);
  success = stan::proto::write_delimited_pb(&pb, pb_ostream);
  EXPECT_EQ(true, success);
  delete pb_ostream;
  close(fd);

  fd = open("/tmp/test-rwDelimitedPb.pb", O_RDONLY); 
  pb_istream = new google::protobuf::io::FileInputStream(fd);
  success = stan::proto::read_delimited_pb(&pb2, pb_istream);
  EXPECT_EQ(true, success);
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
