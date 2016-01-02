#include <iostream>
#include <sstream>
#include <string>
#include <stan/proto/stanc.pb.h>
#include <stan/lang/compiler.hpp>

namespace stan {
  namespace proto {
    stan::proto::StanCompileResponse compile(
      const stan::proto::StanCompileRequest& request) {

      stan::proto::StanCompileResponse response;
      std::ostringstream err_stream;
      std::istringstream stan_stream(request.model_code());
      std::ostringstream cpp_stream;

      try {
        bool valid_model = stan::lang::compile(&err_stream,
          stan_stream, cpp_stream,
          request.model_name(), request.model_file_name());
        if (valid_model) {
          response.set_state(stan::proto::StanCompileResponse::SUCCESS);
        } else {
          response.set_state(stan::proto::StanCompileResponse::ERROR);
        }
      } catch(const std::exception& e) {
        response.set_state(stan::proto::StanCompileResponse::ERROR);
      }
      response.set_messages(err_stream.str());
      response.set_cpp_code(cpp_stream.str());
      return response;
    }
  }
}
