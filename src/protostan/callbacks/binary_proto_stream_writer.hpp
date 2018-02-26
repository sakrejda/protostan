#ifndef STAN_INTERFACE_CALLBACKS_WRITER_BINARY_PROTO_STREAM_WRITER_HPP
#define STAN_INTERFACE_CALLBACKS_WRITER_BINARY_PROTO_STREAM_WRITER_HPP

#include <google/protobuf/io/zero_copy_stream.h>
#include <stan/proto/writer.pb.h>
#include <stan/callbacks/writer.hpp>
#include <protostan/util/rw_delimited_pb.hpp>
#include <ostream>
#include <vector>
#include <string>

namespace stan {
  namespace callbacks {

    /**
     * binary_proto_stream_writer writes to an std::ostream.
     */
    template <bool (*F)(
      google::protobuf::MessageLite*, google::protobuf::io::ZeroCopyOutputStream*)>
    class binary_proto_stream_writer : public writer {
    public:
      /**
       * Constructor. Takes ownership of the pointer argument.
       *
       * @param output pointer to a ZeroCopyOutputStream (sublcass) to
       *               write to.  The object now owns the pointer and
       *               will delete on destruction.
       */
      binary_proto_stream_writer(
        google::protobuf::io::ZeroCopyOutputStream* output) :
        raw_output__(output), iteration__(1) {}

      ~binary_proto_stream_writer() {
        delete raw_output__;
      }

      void operator()(const std::vector<std::string>& names) {
        if (names.empty()) return;

        stan_message__.set_type(stan::proto::StanMessage::PARAMETER_NAMES);
        for (std::vector<std::string>::const_iterator it = names.begin();
             it != names.end(); ++it) {
          stan_message__.mutable_stan_string_output()->set_value(*it);
          write();
        }
      }

      void operator()(const std::vector<double>& state) {
        if (state.empty()) return;

        stan_message__.set_type(stan::proto::StanMessage::PARAMETER_VALUES);
	stan_message__.mutable_stan_parameter_output()->set_iteration(iteration__++);
        for (std::vector<double>::const_iterator it = state.begin();
             it != state.end(); ++it) {
          stan_message__.mutable_stan_parameter_output()->set_value(*it);
          write();
        }
      }

      void operator()() {}

      void operator()(const std::string& message) {
        stan_message__.set_type(stan::proto::StanMessage::MESSAGE);
        stan_message__.mutable_stan_string_output()->set_stan_message(message);
        write();
      }

    private:
      google::protobuf::io::ZeroCopyOutputStream* raw_output__;
      stan::proto::StanMessage stan_message__;
      bool success__;
      int iteration__;

      void write() {
        success__ = F(&stan_message__, raw_output__);
      }
    };

  }
}

#endif
