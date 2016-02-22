#ifndef STAN_INTERFACE_CALLBACKS_WRITER_JSONLINES_WRITER_HPP
#define STAN_INTERFACE_CALLBACKS_WRITER_JSONLINES_WRITER_HPP

#include <stan/proto/sample.pb.h>
#include <stan/interface_callbacks/writer/base_writer.hpp>
#include <ostream>
#include <vector>
#include <string>

namespace stan {
  namespace interface_callbacks {
    namespace writer {

      /**
       * binary_proto_stream_writer_writer writes to an std::ostream.
       */
      class binary_proto_stream_writer_writer : public base_writer {
      public:
        /**
         * Constructor.
         *
         * @param output std::ostream to write to
         * @param key_value_prefix String to write before lines
         *   treated as comments.
         */
        binary_proto_stream_writer_writer(std::ostream& output,
                      const std::string& key_value_prefix = ""):
          output__(output), key_value_prefix__(key_value_prefix) {}

        void operator()(const std::string& key, double value) {
          //output__ << key_value_prefix__ << key << " = " << value << std::endl;
          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan::proto::StanIntegerOutput parameter_output;
          parameter_output.set_key(key); 
          parameter_output.set_value(values);
          stan_message__.set_allocated_stan_parameter_output(&parameter_output);
          write_delimited_pb(stan_message__);
          stan_message__.clear_stan_parameter_output();
        }

        void operator()(const std::string& key, int value) {
          //output__ << key_value_prefix__ << key << " = " << value << std::endl;
          stan_message__.set_type(stan::proto::StanMessage::INTEGER_OUTPUT);
          stan::proto::StanIntegerOutput integer_output;
          integer_output.set_key(key); 
          integer_output.set_value(values);
          stan_message__.set_allocated_stan_integer_output(&integer_output);
          write_delimited_pb(stan_message__);
          stan_message__.clear_stan_integer_output();
        }

        void operator()(const std::string& key, const std::string& value) {
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan::proto::StanStringOutput string_output;
          string_output.set_key(key);
          string_output.set_value(message);
          stan_message__.set_allocated_stan_string_output(&string_output);
          write_delimited_pb(stan_message__);
          stan_message__.clear_stan_string_output();
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
            stan_message__.set_allocated_stan_parameter_output(&parameter_output);
            write_delimited_pb(stan_message__);
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
              //key, i, j, values[i * n_cols + j];
              parameter_output.set_indexing(0, i);
              parameter_output.set_indexing(1, j);
              parameter_output.set_value(values[i * n_cols + j]);
              stan_message__.set_allocated_stan_parameter_output(&parameter_output);
              write_delimited_pb(stan_message__);
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
            write_delimited_pb(stan_message__);
            stan_message__.clear_stan_string_output();
          }
        }

        void operator()(const std::vector<double>& state) {
          if (state.empty()) return;
          int idx = 0;

          for (std::vector<double>::const_iterator it = state.begin();
               it != names.end(); ++it){
            ++idx;
            stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
            stan::proto::StanParameterOutput parameter_output;
            parameter_output.set_key("value"); /// FIXME: store names from above, reuse here.
            parameter_output.set_indexing(0, idx);
            parameter_output.set_value(*it);
            stan_message__.set_allocated_stan_parameter_output(&parameter_output);
            write_delimited_pb(stan_message__);
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
          write_delimited_pb(stan_message__);
          stan_message__.clear_stan_string_output();
        }

      private:
//        std::ostream& output__;
        google::protobuf::io::OstreamOutputStream output__;
        std::string key_value_prefix__;
        stan::proto::StanMessage stan_message__;

        // This follows: http://stackoverflow.com/a/22927149
        // written by Kenton Varda.  
        bool write_delimited_pb(
          const google::protobuf::MessageLite& message
        ) {
          google::protobuf::io::CodedOutputStream output(output__);
          const int size = message.ByteSize();
          output.WriteVarint32(size); 
          
          uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
          if (buffer != NULL ) {
            message.SerializeWithCachedSizesToArray(buffer);
          } else {
            message.SerializeWithCachedSizes(&output);
            if (output.HadError()) 
              return false;
          }
          return true;
        }
      };

    }
  }
}

#endif
