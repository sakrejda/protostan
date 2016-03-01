
#include <iostream>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

namespace stan {
  namespace proto {

    bool write_delimited_pb(
        const google::protobuf::MessageLite& message,
        google::protobuf::io::ZeroCopyOutputStream* raw_output
    ) {
      google::protobuf::io::CodedOutputStream output(raw_output);
      const int size = message.ByteSize();
      std::cout << "SIZE: " << size << std::endl;
      //output.WriteVarint32(size);

      uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
      if (buffer != NULL) {
        message.SerializeWithCachedSizesToArray(buffer);
      } else {
        message.SerializeWithCachedSizes(&output);
        if (output.HadError())
          return false;
      }
      return true;
    }

    bool read_delimited_pb(
        google::protobuf::MessageLite* message,
        google::protobuf::io::ZeroCopyInputStream* raw_input
    ) {
      google::protobuf::io::CodedInputStream input(raw_input);

      google::protobuf::uint32 size;
      std::cout << "SIZE: " << size << std::endl;
      input.ReadVarint32(&size);
      std::cout << "SIZE: " << size << std::endl;
      if (!input.ReadVarint32(&size)) return false;
      std::cout << "READ VARINT!" << std::endl;

      google::protobuf::io::CodedInputStream::Limit limit =
        input.PushLimit(size);

      if (!message->MergeFromCodedStream(&input)) return false;
      if (!input.ConsumedEntireMessage()) return false;

      input.PopLimit(limit);
      return true;  
    };


  }  // proto namespace
}  // stan namespace
