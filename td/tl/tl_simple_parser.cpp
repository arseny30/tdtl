#include "td/tl/tl_simple_parser.h"

namespace td {
namespace tl {
tl_simple_parser::tl_simple_parser(const char *data, std::size_t data_len)
    : data(data), data_begin(data), data_len(data_len), error(NULL), error_pos(0) {
}
}  // namespace tl
}  // namespace td
