#pragma once

#include <string>

#include "tl_outputer.h"

namespace td {
namespace tl {

class tl_string_outputer : public tl_outputer {
  std::string result;

 public:
  virtual void append(const std::string &str);

  const std::string &get_result() const;
};

}  // namespace tl
}  // namespace td
