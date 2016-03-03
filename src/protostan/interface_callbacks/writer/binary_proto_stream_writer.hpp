#ifndef STAN_INTERFACE_CALLBACKS_WRITER_BINARY_PROTO_STREAM_WRITER_HPP
#define STAN_INTERFACE_CALLBACKS_WRITER_BINARY_PROTO_STREAM_WRITER_HPP

#include <google/protobuf/io/zero_copy_stream.h>
#include <stan/proto/sample.pb.h>
#include <protostan/util/rw_delimited_pb.hpp>
#include <stan/interface_callbacks/writer/base_writer.hpp>
#include <ostream>
#include <vector>
#include <string>

namespace stan {
  namespace interface_callbacks {
    namespace writer {

      /**
       * binary_proto_stream_writer writes to an std::ostream.
       */
      class binary_proto_stream_writer : public base_writer {
      public:
        /**
         * Constructor. Takes ownership of the pointer argument.
         *
         * @param output pointer to std::ostream to write to.  The
         *               object now owns the pointer and will delete on
         *               destruction.  
         */
        binary_proto_stream_writer(std::ostream* output) {
          raw_output__ = new google::protobuf::io::OstreamOutputStream(output);   
          flusher__ = output;
        }

        ~binary_proto_stream_writer() {
          flusher__->flush();
          delete raw_output__;
          delete flusher__;
        }

        void operator()(const std::string& key, double value) {
          bool success;
          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->set_value(value);
          success = write_delimited_pb(stan_message__, raw_output__);
        }

        void operator()(const std::string& key, int value) {
          bool success;
          stan_message__.set_type(stan::proto::StanMessage::INTEGER_OUTPUT);
          stan_message__.mutable_stan_integer_output()->set_key(key);
          stan_message__.mutable_stan_integer_output()->set_value(value);
          success = write_delimited_pb(stan_message__, raw_output__);
        }

        void operator()(const std::string& key, const std::string& value) {
          bool success;
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key(key);
          stan_message__.mutable_stan_string_output()->set_value(value);
          success = write_delimited_pb(stan_message__, raw_output__);
        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_values) {
          bool success;
          if (n_values == 0) return;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->add_indexing(0);

          for (int64_t n = 0; n < n_values; ++n) {
            stan_message__.mutable_stan_parameter_output()->set_indexing(0,n);
            stan_message__.mutable_stan_parameter_output()->set_value(values[n]);
            success = write_delimited_pb(stan_message__, raw_output__);
          }

        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_rows, int n_cols) {
          bool success;
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
              success = write_delimited_pb(stan_message__, raw_output__);
            }
          }
        }

        void operator()(const std::vector<std::string>& names) {
          bool success;
          if (names.empty()) return;
          int idx = 0;

          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key("name");
          stan_message__.mutable_stan_string_output()->add_indexing(-1);

          for (std::vector<std::string>::const_iterator it = names.begin();
               it != names.end(); ++it) {
            stan_message__.mutable_stan_string_output()->set_indexing(0, idx);
            stan_message__.mutable_stan_string_output()->set_value(*it);
            success = write_delimited_pb(stan_message__, raw_output__);
            ++idx;
          }
        }

        void operator()(const std::vector<double>& state) {
          bool success;
          if (state.empty()) return;
          int idx = 0;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key("value");
          stan_message__.mutable_stan_parameter_output()->add_indexing(-1);

          for (std::vector<double>::const_iterator it = state.begin();
               it != state.end(); ++it) {
            stan_message__.mutable_stan_parameter_output()->set_indexing(0, idx);
            stan_message__.mutable_stan_parameter_output()->set_value(*it);
            success = write_delimited_pb(stan_message__, raw_output__);
            ++idx;
          }
        }

        void operator()() { }

        void operator()(const std::string& message) {
          bool success;
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key("message");
          stan_message__.mutable_stan_string_output()->set_value(message);
          success = write_delimited_pb(stan_message__, raw_output__);
        }

      private:
        std::ostream* flusher__;
        google::protobuf::io::ZeroCopyOutputStream* raw_output__; 
        stan::proto::StanMessage stan_message__;

      };

    }
  }
}

#endif
