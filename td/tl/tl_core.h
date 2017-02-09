#pragma once

#include <cstring>
#include <cstdint>

#include <string>
#include <vector>

namespace td {
namespace tl {

const int NODE_TYPE_TYPE = 1;
const int NODE_TYPE_NAT_CONST = 2;
const int NODE_TYPE_VAR_TYPE = 3;
const int NODE_TYPE_VAR_NUM = 4;
const int NODE_TYPE_ARRAY = 5;

const int32_t ID_VAR_NUM = 0x70659eff;
const int32_t ID_VAR_TYPE = 0x2cecf817;
const int32_t ID_INT = 0xa8509bda;
const int32_t ID_LONG = 0x22076cba;
const int32_t ID_DOUBLE = 0x2210c154;
const int32_t ID_STRING = 0xb5286e24;
const int32_t ID_VECTOR = 0x1cb5c415;
const int32_t ID_DICTIONARY = 0x1f4c618f;
const int32_t ID_MAYBE_TRUE = 0x3f9c8ef8;
const int32_t ID_MAYBE_FALSE = 0x27930a7b;
const int32_t ID_BOOL_FALSE = 0xbc799737;
const int32_t ID_BOOL_TRUE = 0x997275b5;

const int FLAG_OPT_VAR = (1 << 17);
const int FLAG_EXCL = (1 << 18);
const int FLAG_NOVAR = (1 << 21);
const int FLAG_DEFAULT_CONSTRUCTOR = (1 << 25);
const int FLAG_BARE = (1 << 0);
const int FLAG_COMPLEX = (1 << 1);
const int FLAGS_MASK = ((1 << 16) - 1);

class tl_combinator;

class tl_tree;

class tl_type {
 public:
  int32_t id;
  std::string name;
  int arity;
  int flags;
  int simple_constructors;
  int constructors_num;
  std::vector<tl_combinator *> constructors;

  void add_constructor(tl_combinator *new_constructor);

  int get_constructor_num(const std::string &name) const;

  int get_constructor_num(int id) const;
};

class arg {
 public:
  std::string name;
  int flags;
  int var_num;
  int exist_var_num;
  int exist_var_bit;
  tl_tree *type;
};

class tl_combinator {
 public:
  int32_t id;
  std::string name;
  int var_count;
  int32_t type_id;
  std::vector<arg> args;
  tl_tree *result;
};

class tl_tree {
 public:
  int flags;

  explicit tl_tree(int flags) : flags(flags) {
  }

  virtual int get_type() const = 0;

  virtual ~tl_tree() {
  }
};

class tl_tree_type : public tl_tree {
 public:
  tl_type *type;
  std::vector<tl_tree *> children;

  tl_tree_type(int flags, tl_type *type, int child_count) : tl_tree(flags), type(type), children(child_count) {
  }

  virtual int get_type() const {
    return NODE_TYPE_TYPE;
  }
};

class tl_tree_nat_const : public tl_tree {
 public:
  int num;

  tl_tree_nat_const(int flags, int num) : tl_tree(flags), num(num) {
  }

  virtual int get_type() const {
    return NODE_TYPE_NAT_CONST;
  }
};

class tl_tree_var_type : public tl_tree {
 public:
  int var_num;

  tl_tree_var_type(int flags, int var_num) : tl_tree(flags), var_num(var_num) {
  }

  virtual int get_type() const {
    return NODE_TYPE_VAR_TYPE;
  }
};

class tl_tree_var_num : public tl_tree {
 public:
  int var_num;
  int diff;

  tl_tree_var_num(int flags, int var_num, int diff) : tl_tree(flags), var_num(var_num), diff(diff) {
  }

  virtual int get_type() const {
    return NODE_TYPE_VAR_NUM;
  }
};

class tl_tree_array : public tl_tree {
 public:
  tl_tree *multiplicity;
  std::vector<arg> args;

  tl_tree_array(int flags, tl_tree *multiplicity, int arg_count)
      : tl_tree(flags), multiplicity(multiplicity), args(arg_count) {
  }

  tl_tree_array(int flags, tl_tree *multiplicity, const std::vector<arg> &a)
      : tl_tree(flags), multiplicity(multiplicity), args(a) {
  }

  virtual int get_type() const {
    return NODE_TYPE_ARRAY;
  }
};

}  // namespace tl
}  // namespace td
