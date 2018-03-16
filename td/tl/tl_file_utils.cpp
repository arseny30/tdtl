#include "tl_file_utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace td {
namespace tl {

std::string get_file_contents(const std::string &file_name, const std::string &mode) {
  FILE *f = std::fopen(file_name.c_str(), mode.c_str());
  if (f == NULL) {
    return std::string();
  }

  int fseek_res = std::fseek(f, 0, SEEK_END);
  if (fseek_res != 0) {
    std::fprintf(stderr, "Can't seek to the end of the file \"%s\"", file_name.c_str());
    std::abort();
  }
  long size_long = std::ftell(f);
  if (size_long < 0 || size_long >= (1 << 25)) {
    std::fprintf(stderr, "Wrong file \"%s\" has wrong size = %ld", file_name.c_str(), size_long);
    std::abort();
  }
  std::size_t size = static_cast<std::size_t>(size_long);

  std::string result(size, ' ');
  std::rewind(f);
  std::size_t fread_res = std::fread(&result[0], size, 1, f);
  if (size != 0 && fread_res != 1) {
    std::fprintf(stderr, "Can't read file \"%s\"", file_name.c_str());
    std::abort();
  }
  std::fclose(f);

  return result;
}

bool put_file_contents(const std::string &file_name, const std::string &mode, const std::string &contents) {
  FILE *f = std::fopen(file_name.c_str(), mode.c_str());
  if (f == NULL) {
    std::fprintf(stderr, "Can't open file \"%s\"\n", file_name.c_str());
    return false;
  }

  std::size_t fwrite_res = std::fwrite(contents.c_str(), contents.size(), 1, f);
  if (fwrite_res != 1) {
    std::fclose(f);
    return false;
  }
  if (std::fclose(f) != 0) {
    return false;
  }
  return true;
}

std::string remove_documentation(const std::string &str) {
  std::size_t line_begin = 0;
  std::string result;
  bool inside_documentation = false;
  while (line_begin < str.size()) {
    std::size_t line_end = str.find('\n', line_begin);
    if (line_end == std::string::npos) {
      line_end = str.size() - 1;
    }
    std::string line = str.substr(line_begin, line_end - line_begin + 1);
    line_begin = line_end + 1;

    std::size_t pos = line.find_first_not_of(' ');
    if (pos != std::string::npos && ((line[pos] == '/' && line[pos + 1] == '/' && line[pos + 2] == '/') ||
                                     (line[pos] == '/' && line[pos + 1] == '*' && line[pos + 2] == '*') ||
                                     (inside_documentation && line[pos] == '*'))) {
      inside_documentation = !(line[pos] == '/' && line[pos + 1] == '/' && line[pos + 2] == '/') &&
                             !(line[pos] == '*' && line[pos + 1] == '/');
      continue;
    }

    inside_documentation = false;
    result += line;
  }
  return result;
}

}  // namespace tl
}  // namespace td
