#ifndef STAN_UTIL_RW_DELIMITED_PB_HPP
#define STAN_UTIL_RW_DELIMITED_PB_HPP

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <iostream>
#include <string>

namespace stan {
  namespace proto {

    bool write_delimited_pb(
        google::protobuf::MessageLite* message,
        google::protobuf::io::ZeroCopyOutputStream* raw_output
    ) {
      google::protobuf::io::CodedOutputStream output(raw_output);
      const google::protobuf::uint32 size = message->ByteSize();
      output.WriteVarint32(size);
      message->SerializeToCodedStream(&output);
      return true;
    }

    bool read_delimited_pb(
        google::protobuf::MessageLite* message,
        google::protobuf::io::ZeroCopyInputStream* raw_input
    ) {
      google::protobuf::io::CodedInputStream input(raw_input);

      google::protobuf::uint32 size = 0;
      if (!input.ReadVarint32(&size)) return false;

      google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);

      message->Clear();
      if (!message->MergeFromCodedStream(&input)) return false;
      if (!input.ConsumedEntireMessage()) return false;
      input.PopLimit(limit);
      return true;
    }

  }  // proto namespace
}  // stan namespace

#endif
