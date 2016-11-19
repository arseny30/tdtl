#include <string>

#include "tl_string_outputer.h"

namespace td {
namespace tl {

void tl_string_outputer::append(const std::string &str) {
  result += str;
}

const std::string &tl_string_outputer::get_result() const {
  return result;
}

}  // namespace tl
}  // namespace td
