#include <cassert>

#include <string>

#include "tl_core.h"

namespace td {
namespace tl {

void tl_type::add_constructor(tl_combinator *new_constructor) {
  constructors.push_back(new_constructor);

  assert((int)constructors.size() <= constructors_num);
}

int tl_type::get_constructor_num(const std::string &name) const {
  for (int i = 0; i < constructors_num; i++) {
    if (constructors[i]->name == name) {
      return i;
    }
  }
  return -1;
}

int tl_type::get_constructor_num(int id) const {
  for (int i = 0; i < constructors_num; i++) {
    if (constructors[i]->id == id) {
      return i;
    }
  }
  return -1;
}

}  // namespace tl
}  // namespace td
