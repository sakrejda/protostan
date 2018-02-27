#ifndef STAN_CALLBACKS_CAPN_LOGGER_HPP
#define STAN_CALLBACKS_CAPN_LOGGER_HPP

#include <google/protobuf/io/zero_copy_stream.h>
#include <stan/callbacks/logger.hpp>
#include <stan/proto/logger.pb.h>
#include <ostream>
#include <string>
#include <sstream>

namespace stan {
  namespace callbacks {

    /**
     * <code>protobuf_logger</code> is an implementation of
     * <code>logger</code> that writes messages to a
     * binary output file in a protobuf sformat
     * See schema for details and protobuf doc for how to
     * use the format.  
     */
    template <bool (*F)(
      google::protobuf::MessageLite*, google::protobuf::io::ZeroCopyOutputStream*)>
    class binary_proto_stream_logger : public stan::callbacks::logger {

    public:
      /**
       * Constructs a <code>binary_proto_stream_logger</code> with a 
       * specified run_id and output stream
       *
       * @param[out] output stream to write messages to.
       * @param[in] run_id id used to label data from a single run.
       */
      binary_proto_stream_logger(std::ostream& output, unsigned long run_id)
        : output_(output), run_id__(run_id) { }

      ~binary_proto_stream_writer() {
        delete raw_output__;
      }

      void debug(const std::string& message) {
	log_it(message, stan::proto::StanLog::DEBUG);
      }

      void debug(const std::stringstream& message) {
        debug(message.str());
      }

      void info(const std::string& message) {
	log_it(message, stan::proto::StanLog::INFO);
      }

      void info(const std::stringstream& message) {
        info(message.str());
      }

      void warn(const std::string& message) {
	log_it(message, stan::proto::StanLog::WARN);
      }

      void warn(const std::stringstream& message) {
        warn(message.str());
      }

      void error(const std::string& message) {
	log_it(message, stan::proto::StanLog::ERROR);
      }

      void error(const std::stringstream& message) {
        error(message.str());
      }

      void fatal(const std::string& message) {
	log_it(message, stan::proto::StanLog::FATAL);
      }

      void fatal(const std::stringstream& message) {
        fatal(message.str());
      }

    private:
      unsigned long run_id__;
      google::protobuf::io::ZeroCopyOutputStream* raw_output__;
      stan::proto::StanMessage stan_message__;
      bool success__;

      void write() {
        success__ = F(&stan_message__, raw_output__);
      }

      void log_it(const std::string& message, int level) {
	stan_message__.set_type(stan::proto::StanMessage::LOG);
	stan_message__.set_id(run_id__);
	auto inner_message = stan_message__.mutable_stan_log_message();
	inner_message->set_level(level);
	inner_message->set_type(stan::proto::StanLog::STRING);
	inner_message->set_log_message(message);
        write();
      }
    };

  }
}
#endif
