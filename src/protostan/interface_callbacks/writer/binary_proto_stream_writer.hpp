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
       * binary_proto_stream_writer writes a stan::proto::StanMessage to
       * a google::protobuf::io::ZeroCopyOutputStream (or subclass).
       * Re-uses a single private message across all its methods to
       * avoid repeated construction/destruction of messages.  The
       * message is Clear()-ed at the beginning of each method to avoid
       * bringing cruft from a previous call.  
       *
       * The output format is basically a key/index/value format and the
       * writer could keep track of additional information (e.g., for
       * parameter output in samples the iteration could be recorded as
       * well.  What additional data should be tracked?
       */
      template <bool (*F)(
        google::protobuf::MessageLite*, google::protobuf::io::ZeroCopyOutputStream*)>
      class binary_proto_stream_writer : public base_writer {
      public:
        /**
         * Constructor.
         *
         * @param[in] output pointer to a ZeroCopyOutputStream, the
         *   ojbect takes ownership (will delete) the stream on
         *   destruction.
         */
        binary_proto_stream_writer(
          google::protobuf::io::ZeroCopyOutputStream* output) : 
          raw_output__(output) {}

        ~binary_proto_stream_writer() {
          delete raw_output__;
        }

        /**
         * The key/value(double) combination is written as PARAMETER_OUTPUT type
         * of message.  I don't really know when this version of the
         * call is currently used. Should there be a specific message
         * type?  Should the writer track more information to add to the
         * message?
         *  @param[in] key 
         *  @param[in] value double to write.
         */
        void operator()(const std::string& key, double value) {
          stan_message__.Clear();
          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->set_value(value);
          write();
        }

        /**
         * The key/value(int) combination is written as INTEGER_OUTPUT type
         * of message.  I don't really know when this version of the
         * call is currently used. Should there be a specific message
         * type?  Should the writer track more information to add to the
         * message?  I think this is used to output info on control
         * (input) parameters in CmdStan...
         *  @param[in] key 
         *  @param[in] value integer to write.
         */
        void operator()(const std::string& key, int value) {
          stan_message__.Clear();
          stan_message__.set_type(stan::proto::StanMessage::INTEGER_OUTPUT);
          stan_message__.mutable_stan_integer_output()->set_key(key);
          stan_message__.mutable_stan_integer_output()->set_value(value);
          write();
        }

        /**
         * The key/value(string) combination is written as STRING_OUTPUT type
         * of message.  I don't really know when this version of the
         * call is currently used. Should there be a specific message
         * type?  Should the writer track more information to add to the
         * message?  I think this is used to output info on control
         * (input) data?
         *  @param[in] key 
         *  @param[in] value integer to write.
         */
        void operator()(const std::string& key, const std::string& value) {
          stan_message__.Clear();
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key(key);
          stan_message__.mutable_stan_string_output()->set_value(value);
          write();
        }

        void operator()(const std::string& key,
                        const double* values,
                        int n_values) {
          if (n_values == 0) return;

          stan_message__.Clear();
          stan_message__.set_type(stan::proto::StanMessage::PARAMETER_OUTPUT);
          stan_message__.mutable_stan_parameter_output()->set_key(key);
          stan_message__.mutable_stan_parameter_output()->add_indexing(0);

          for (int64_t n = 0; n < n_values; ++n) {
            stan_message__.mutable_stan_parameter_output()->set_indexing(0,n);
            stan_message__.mutable_stan_parameter_output()->set_value(values[n]);
            write();
          }

        }

        /**
         * The key/values(double*)/int/int combination is written as
         * PARAMETER_OUTPUT.  I don't think this is used to output
         * sampled parameters but rather write out the mass matrix or
         * something like that.  Should there be a more specific message
         * type?  Additional info to track?
         * @param[in] key
         * @param[in] values pointer to array of values to output.
         * @param[in] n_rows number of rows in the (implied) matrix.
         * @param[in] n_cols number of columns in the (implied) matrix.
         */
        void operator()(const std::string& key,
                        const double* values,
                        int n_rows, int n_cols) {
          if (n_rows == 0 || n_cols == 0) return;

          stan_message__.Clear();
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

        /**
         * The vector<string> operator writes out the names of
         * parameters (in CmdStan anyway).  I think this message type is
         * appropriate and contains enough information.
         * @param [in] names vector of parameter names to write.
         */
        void operator()(const std::vector<std::string>& names) {
          if (names.empty()) return;
          int idx = 0;

          stan_message__.Clear();
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

        /**
         * The vector<double> operator writes out the sampled/optimized
         * parameter values at each iteration/whatever.  This message
         * type could use iteration information added into it.  
         * @param [in]  values of parameters to write.
        void operator()(const std::vector<double>& state) {
          if (state.empty()) return;
          int idx = 0;

          stan_message__.Clear();
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

        /**
         * I'm not even sure why this is here, is there something wrong
         * with leaving it undefined?
         */
        void operator()() { }

        /**
         * The string operator writes out messages, The message type and
         * info seem appropriate, although maybe the program/model name
         * could be included.
        void operator()(const std::string& message) {
          stan_message__.Clear();
          stan_message__.set_type(stan::proto::StanMessage::STRING_OUTPUT);
          stan_message__.mutable_stan_string_output()->set_key("message");
          stan_message__.mutable_stan_string_output()->set_value(message);
          write();
        }

      private:
        google::protobuf::io::ZeroCopyOutputStream* raw_output__; 
        stan::proto::StanMessage stan_message__;
        bool success__;

        /**
         * Here we do the writing with whatever function go passed to
         * the template.  The type is bool:
         *   (*F)(messageLite*, * zeroCopyOutputStream)
         */
        void write() {
          success__ = F(&stan_message__, raw_output__);
        }

      };

    }
  }
}

#endif
