#ifndef STAN_UTIL_RW_DELIMITED_PB_HPP
#define STAN_UTIL_RW_DELIMITED_PB_HPP

#include <string>
#include <iostream>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

namespace stan {
  namespace proto {

    /**
     * Write the message pointed to by the first argument to the stream
     * specified by the second argument, prefixed by a
     * google::protobuf::Varint32 which indicates the length of the
     * message. No copies anywhere.
     *  @param[in] message Pointer to the message to be written out, the
     *    message can be either Message or MessageLite.
     *  @param[in] raw_output Pointer to the stream to write to, the
     *    stream pointed to can be any subclass of ZeroCopyOutputStream. 
     *  @return true on success and false on any failures.
     */
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

    /** 
     * Read data into the message pointed to by the first argument into
     * the input steam pointed to by the second argument.  First a
     * google::protobuf::Varint32 is read from the stream and it
     * indicates how many bytes will be read from the stream.  Then the
     * message data is read from the stream and merged into the message.
     * The message must be cleared prior to merging as otherwise
     * repeated fields are appended rather than replaced.  At the end of
     * hte operation the next item on the stream should be the Varint32
     * indicating the length of the subsequent message.
     *  @param[in] message Pointer to the message to merge data into.
     *  @param[in] raw_input Pointer to the ZeroCopyInputStream to read
     *    from. Stream pointed to can be any subclass of
     *    ZeroCopyInputStream.
     *  @return true on success and false on any failures.
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
    };


  }  // proto namespace
}  // stan namespace

#endif
