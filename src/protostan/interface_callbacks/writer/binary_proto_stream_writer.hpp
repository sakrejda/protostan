#ifndef STAN_INTERFACE_CALLBACKS_WRITER_BINARY_PROTO_STREAM_WRITER_HPP
#define STAN_INTERFACE_CALLBACKS_WRITER_BINARY_PROTO_STREAM_WRITER_HPP

#include <google/protobuf/io/zero_copy_stream.h>
#include <stan/proto/sample.pb.h>
#include <stan/interface_callbacks/writer/base_writer.hpp>
#include <protostan/util/rw_delimited_pb.hpp>
#include <ostream>
#include <vector>
#include <string>

namespace stan {
  namespace interface_callbacks {
    namespace writer {

      /**
       * binary_proto_stream_writer writes to an std::ostream.
       */
      template <bool (*F)(
        google::protobuf::MessageLite*, google::protobuf::io::ZeroCopyOutputStream*)>
      class binary_proto_stream_writer : public base_writer {
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
          raw_output__(output) {}

        ~binary_proto_stream_writer() {
          delete raw_output__;
        }

        void operator()(const std::string& key, double value) {
          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->set_value(value);
          write();
        }

        void operator()(const std::string& key, int value) {
          stan_message__.set_type(stan::proto::StanMessage::INTEGER_OUTPUT);
          stan_message__.mutable_stan_integer_output()->set_key(key);
          stan_message__.mutable_stan_integer_output()->set_value(value);
          write();
        }

        void operator()(const std::string& key, const std::string& value) {
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key(key);
          stan_message__.mutable_stan_string_output()->set_value(value);
          write();
        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_values) {
          if (n_values == 0) return;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->add_indexing(0);

          for (int64_t n = 0; n < n_values; ++n) {
            stan_message__.mutable_stan_parameter_output()->set_indexing(0,n);
            stan_message__.mutable_stan_parameter_output()->set_value(values[n]);
            write();
          }

        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_rows, int n_cols) {
          if (n_rows == 0 || n_cols == 0) return;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->add_indexing(-1);
          stan_message__.mutable_stan_parameter_output()->add_indexing(-1);

          for (int i = 0; i < n_rows; ++i) {
            for (int j = 0; j < n_cols; ++j) {
              stan_message__.mutable_stan_parameter_output()->set_indexing(0, i);
              stan_message__.mutable_stan_parameter_output()->set_indexing(1, j);
              stan_message__.mutable_stan_parameter_output()->set_value(values[i * n_cols + j]);
              write();
            }
          }
        }

        void operator()(const std::vector<std::string>& names) {
          if (names.empty()) return;
          int idx = 0;

          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key("name");
          stan_message__.mutable_stan_string_output()->add_indexing(-1);

          for (std::vector<std::string>::const_iterator it = names.begin();
               it != names.end(); ++it) {
            stan_message__.mutable_stan_string_output()->set_indexing(0, idx);
            stan_message__.mutable_stan_string_output()->set_value(*it);
            write();
            ++idx;
          }
        }

        void operator()(const std::vector<double>& state) {
          if (state.empty()) return;
          int idx = 0;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key("value");
          stan_message__.mutable_stan_parameter_output()->add_indexing(-1);

          for (std::vector<double>::const_iterator it = state.begin();
               it != state.end(); ++it) {
            stan_message__.mutable_stan_parameter_output()->set_indexing(0, idx);
            stan_message__.mutable_stan_parameter_output()->set_value(*it);
            write();
            ++idx;
          }
        }

        void operator()() { }

        void operator()(const std::string& message) {
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key("message");
          stan_message__.mutable_stan_string_output()->set_value(message);
          write();
        }

      private:
        google::protobuf::io::ZeroCopyOutputStream* raw_output__; 
        stan::proto::StanMessage stan_message__;
        bool success__;

        void write() {
          success__ = F(&stan_message__, raw_output__);
        }

      };

    }
  }
}

#endif
