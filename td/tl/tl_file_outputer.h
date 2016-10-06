#pragma once

#include <cstdio>
#include <string>

#include "tl_outputer.h"

namespace td {
namespace tl {

class tl_file_outputer : public tl_outputer {
  FILE *f;

  void close(void);

 public:
  tl_file_outputer(void);

  bool open(const std::string &file_name);

  virtual void append(const std::string &str);

  virtual ~tl_file_outputer(void);
};

}  // namespace tl
}  // namespace td
