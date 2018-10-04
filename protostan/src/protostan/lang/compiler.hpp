#include <stan/proto/compile.pb.h>
#include <stan/lang/compiler.hpp>
#include <iostream>
#include <sstream>
#include <string>

namespace stan {
  namespace proto {
    stan::proto::StanCompileResponse compile(
      const stan::proto::StanCompileRequest& request) {
      stan::proto::StanCompileResponse response;
      std::ostringstream err_stream;
      std::istringstream stan_stream(request.model_code());
      std::ostringstream cpp_stream;

      response.set_state(stan::proto::StanCompileResponse::ERROR);
      try {
        bool valid_model = stan::lang::compile(&err_stream,
                                               stan_stream,
                                               cpp_stream,
                                               request.model_name());
        response.set_messages(err_stream.str());
        if (valid_model) {
          response.set_state(stan::proto::StanCompileResponse::SUCCESS);
          response.set_cpp_code(cpp_stream.str());
        }
      } catch(const std::exception& e) {
        response.set_messages(e.what());
      }
      return response;
    }
  }
}
