#define _CRT_SECURE_NO_WARNINGS
#include "tl_file_outputer.h"

#include <cassert>
#include <cassert>

namespace td {
namespace tl {

void tl_file_outputer::append(const std::string &str) {
  assert(f != NULL);
  fprintf(f, "%s", str.c_str());
}

tl_file_outputer::tl_file_outputer() : f(NULL) {
}

void tl_file_outputer::close() {
  if (f) {
    fclose(f);
  }
}

bool tl_file_outputer::open(const std::string &file_name) {
  close();

  f = fopen(file_name.c_str(), "w");

  return (f != NULL);
}

tl_file_outputer::~tl_file_outputer() {
  close();
}

}  // namespace tl
}  // namespace td
