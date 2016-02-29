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
         * Constructor.
         *
         * @param output std::ostream to write to
         */
        binary_proto_stream_writer(std::ostream& output) {
          raw_output__ = new google::protobuf::io::OstreamOutputStream(&output);   
        }

        ~binary_proto_stream_writer() {
          delete raw_output__;
        }

        void operator()(const std::string& key, double value) {
          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan::proto::StanParameterOutput parameter_output;
          parameter_output.set_key(key);
          parameter_output.set_value(value);
          stan_message__.set_allocated_stan_parameter_output(&parameter_output);
          write_delimited_pb(stan_message__, raw_output__);
          stan_message__.clear_stan_parameter_output();
        }

        void operator()(const std::string& key, int value) {
          stan_message__.set_type(stan::proto::StanMessage::INTEGER_OUTPUT);
          stan::proto::StanIntegerOutput integer_output;
          integer_output.set_key(key);
          integer_output.set_value(value);
          stan_message__.set_allocated_stan_integer_output(&integer_output);
          write_delimited_pb(stan_message__, raw_output__);
          stan_message__.clear_stan_integer_output();
        }

        void operator()(const std::string& key, const std::string& value) {
          bool success;
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan::proto::StanStringOutput* string_output(stan_message__.mutable_stan_string_output());
          string_output->set_key(key);
          string_output->set_value(value);
          std::cout << "BLAH: " << stan_message__.stan_string_output().key() << std::endl;
          std::cout << "BLAH: " << stan_message__.stan_string_output().value() << std::endl;
          success = write_delimited_pb(stan_message__, raw_output__);
          if (!success) throw("Write failed.");
        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_values) {
          if (n_values == 0) return;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan::proto::StanParameterOutput parameter_output;
          parameter_output.set_key(key);

          for (int n = 0; n < n_values; ++n) {
            parameter_output.set_indexing(0, n);
            parameter_output.set_value(values[n]);
            stan_message__.set_allocated_stan_parameter_output(
              &parameter_output);
            write_delimited_pb(stan_message__, raw_output__);
            stan_message__.clear_stan_parameter_output();
          }
        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_rows, int n_cols) {
          if (n_rows == 0 || n_cols == 0) return;

          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan::proto::StanParameterOutput parameter_output;
          parameter_output.set_key(key);


          for (int i = 0; i < n_rows; ++i) {
            for (int j = 0; j < n_cols; ++j) {
              parameter_output.set_indexing(0, i);
              parameter_output.set_indexing(1, j);
              parameter_output.set_value(values[i * n_cols + j]);
              stan_message__.set_allocated_stan_parameter_output(
                &parameter_output);
              write_delimited_pb(stan_message__, raw_output__);
              stan_message__.clear_stan_parameter_output();
            }
          }
        }

        void operator()(const std::vector<std::string>& names) {
          if (names.empty()) return;
          int idx = 0;

          for (std::vector<std::string>::const_iterator it = names.begin();
               it != names.end(); ++it) {
            ++idx;
            stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
            stan::proto::StanStringOutput string_output;
            string_output.set_key("name");
            string_output.set_indexing(0, idx);
            string_output.set_value(*it);
            stan_message__.set_allocated_stan_string_output(&string_output);
            write_delimited_pb(stan_message__, raw_output__);
            stan_message__.clear_stan_string_output();
          }
        }

        void operator()(const std::vector<double>& state) {
          if (state.empty()) return;
          int idx = 0;

          for (std::vector<double>::const_iterator it = state.begin();
               it != state.end(); ++it) {
            ++idx;
            stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
            stan::proto::StanParameterOutput parameter_output;
            parameter_output.set_key("value");
            parameter_output.set_indexing(0, idx);
            parameter_output.set_value(*it);
            stan_message__.set_allocated_stan_parameter_output(
              &parameter_output);
            write_delimited_pb(stan_message__, raw_output__);
            stan_message__.clear_stan_parameter_output();
          }
        }

        void operator()() { }

        void operator()(const std::string& message) {
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan::proto::StanStringOutput string_output;
          string_output.set_key("message");
          string_output.set_value(message);
          stan_message__.set_allocated_stan_string_output(&string_output);
          write_delimited_pb(stan_message__, raw_output__);
          stan_message__.clear_stan_string_output();
        }

      private:
        google::protobuf::io::ZeroCopyOutputStream* raw_output__; 
        stan::proto::StanMessage stan_message__;

      };

    }
  }
}

#endif
