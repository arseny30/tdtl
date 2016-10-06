#include <cassert>
#include <cstdio>

#include "tl_config.h"

namespace td {
namespace tl {

const int TLS_SCHEMA_V2 = 0x3a2f9be2;
const int TLS_SCHEMA_V3 = 0xe4a8604b;
const int TLS_SCHEMA_V4 = 0x90ac88d7;
const int TLS_TYPE = 0x12eb4386;
const int TLS_COMBINATOR = 0x5c0a1ed5;
const int TLS_COMBINATOR_LEFT_BUILTIN = 0xcd211f63;
const int TLS_COMBINATOR_LEFT = 0x4c12c6d9;
const int TLS_COMBINATOR_RIGHT_V2 = 0x2c064372;
const int TLS_ARG_V2 = 0x29dfe61b;

const int TLS_EXPR_NAT = 0xdcb49bd8;
const int TLS_EXPR_TYPE = 0xecc9da78;

const int TLS_NAT_CONST_OLD = 0xdcb49bd8;
const int TLS_NAT_CONST = 0x8ce940b1;
const int TLS_NAT_VAR = 0x4e8a14f0;
const int TLS_TYPE_VAR = 0x0142ceae;
const int TLS_ARRAY = 0xd9fb20de;
const int TLS_TYPE_EXPR = 0xc1863d08;

void tl_config::add_type(tl_type *type) {
  types.push_back(type);
  id_to_type[type->id] = type;
  name_to_type[type->name] = type;
}

tl_type *tl_config::get_type(int type_id) {
  return id_to_type[type_id];
}

tl_type *tl_config::get_type(const std::string &type_name) {
  return name_to_type[type_name];
}

void tl_config::add_function(tl_combinator *function) {
  functions.push_back(function);
  id_to_function[function->id] = function;
  name_to_function[function->name] = function;
}

tl_combinator *tl_config::get_function(int function_id) {
  return id_to_function[function_id];
}

tl_combinator *tl_config::get_function(const std::string &function_name) {
  return name_to_function[function_name];
}

int tl_config::get_type_count(void) const {
  return static_cast<int>(types.size());
}

tl_type *tl_config::get_type_by_num(int num) const {
  return types[num];
}

int tl_config::get_function_count(void) const {
  assert(functions.size() < 1u << 30);
  return static_cast<int>(functions.size());
}

tl_combinator *tl_config::get_function_by_num(int num) const {
  return functions[num];
}

int32_t tl_config_parser::try_parse_int(void) {
  return try_parse(p.fetch_int());
}

int64_t tl_config_parser::try_parse_long(void) {
  return try_parse(p.fetch_long());
}

std::string tl_config_parser::try_parse_string(void) {
  return try_parse(p.fetch_string<std::string>());
}

template <class T>
T tl_config_parser::try_parse(const T &res) const {
  if (p.get_error() != NULL) {
    fprintf(stderr, "Wrong TL-scheme specified: %s at %d\n", p.get_error(), p.get_error_pos());
    assert(0);
    exit(1);
  }

  return res;
}

int tl_config_parser::get_schema_version(int a) {
  if (a == TLS_SCHEMA_V4) {
    return 4;
  }
  if (a == TLS_SCHEMA_V3) {
    return 3;
  }
  if (a == TLS_SCHEMA_V2) {
    return 2;
  }
  return -1;
}

tl_tree *tl_config_parser::read_num_const(void) {
  int num = try_parse_int();

  return new tl_tree_nat_const(FLAG_NOVAR, num);
}

tl_tree *tl_config_parser::read_num_var(int *var_count) {
  int diff = try_parse_int();
  int var_num = try_parse_int();

  if (var_num >= *var_count) {
    *var_count = var_num + 1;
  }

  return new tl_tree_var_num(0, var_num, diff);
}

tl_tree *tl_config_parser::read_type_var(int *var_count) {
  int var_num = try_parse_int();
  int flags = try_parse_int();

  if (var_num >= *var_count) {
    *var_count = var_num + 1;
  }
  assert(!(flags & (FLAG_NOVAR | FLAG_BARE)));

  return new tl_tree_var_type(flags, var_num);
}

tl_tree *tl_config_parser::read_array(int *var_count) {
  int flags = FLAG_NOVAR;
  tl_tree *multiplicity = read_nat_expr(var_count);

  tl_tree_array *T = new tl_tree_array(flags, multiplicity, read_args_list(var_count));

  for (int i = 0; i < (int)T->args.size(); i++) {
    if (!(T->args[i].flags & FLAG_NOVAR)) {
      T->flags &= ~FLAG_NOVAR;
    }
  }
  return T;
}

tl_tree *tl_config_parser::read_type(int *var_count) {
  tl_type *type = config.get_type(try_parse_int());
  assert(type != NULL);
  int flags = try_parse_int() | FLAG_NOVAR;
  int arity = try_parse_int();
  assert(type->arity == arity);

  tl_tree_type *T = new tl_tree_type(flags, type, arity);
  for (int i = 0; i < arity; i++) {
    tl_tree *child = read_expr(var_count);

    T->children[i] = child;
    if (!(child->flags & FLAG_NOVAR)) {
      T->flags &= ~FLAG_NOVAR;
    }
  }
  return T;
}

tl_tree *tl_config_parser::read_type_expr(int *var_count) {
  int tree_type = try_parse_int();
  switch (tree_type) {
    case TLS_TYPE_VAR:
      return read_type_var(var_count);
    case TLS_TYPE_EXPR:
      return read_type(var_count);
    case TLS_ARRAY:
      return read_array(var_count);
    default:
      fprintf(stderr, "tree_type = %d\n", tree_type);
      assert(0);
      return NULL;
  }
}

tl_tree *tl_config_parser::read_nat_expr(int *var_count) {
  int tree_type = try_parse_int();
  switch (tree_type) {
    case TLS_NAT_CONST_OLD:
    case TLS_NAT_CONST:
      return read_num_const();
    case TLS_NAT_VAR:
      return read_num_var(var_count);
    default:
      fprintf(stderr, "tree_type = %d\n", tree_type);
      assert(0);
      return NULL;
  }
}

tl_tree *tl_config_parser::read_expr(int *var_count) {
  int tree_type = try_parse_int();
  switch (tree_type) {
    case TLS_EXPR_NAT:
      return read_nat_expr(var_count);
    case TLS_EXPR_TYPE:
      return read_type_expr(var_count);
    default:
      fprintf(stderr, "tree_type = %d\n", tree_type);
      assert(0);
      return NULL;
  }
}

std::vector<arg> tl_config_parser::read_args_list(int *var_count) {
  const int schema_flag_opt_field = 2 << static_cast<int>(schema_version >= 3);
  const int schema_flag_has_vars = schema_flag_opt_field ^ 6;

  int args_num = try_parse_int();
  std::vector<arg> args(args_num);
  for (int i = 0; i < args_num; i++) {
    arg cur_arg;

    int32_t arg_v = try_parse_int();
    assert(arg_v == TLS_ARG_V2);

    cur_arg.name = try_parse_string();
    cur_arg.flags = try_parse_int();

    bool is_optional = false;
    if (cur_arg.flags & schema_flag_opt_field) {
      cur_arg.flags &= ~schema_flag_opt_field;
      is_optional = true;
    }
    if (cur_arg.flags & schema_flag_has_vars) {
      cur_arg.flags &= ~schema_flag_has_vars;
      cur_arg.var_num = try_parse_int();
    } else {
      cur_arg.var_num = -1;
    }

    if (cur_arg.var_num >= *var_count) {
      *var_count = cur_arg.var_num + 1;
    }
    if (is_optional) {
      cur_arg.exist_var_num = try_parse_int();
      cur_arg.exist_var_bit = try_parse_int();
    } else {
      cur_arg.exist_var_num = -1;
      cur_arg.exist_var_bit = 0;
    }
    cur_arg.type = read_type_expr(var_count);
    if (/*cur_arg.var_num < 0 && cur_arg.exist_var_num < 0 && */ (cur_arg.type->flags & FLAG_NOVAR)) {
      cur_arg.flags |= FLAG_NOVAR;
    }

    args[i] = cur_arg;
  }
  return args;
}

tl_combinator *tl_config_parser::read_combinator(void) {
  int32_t t = try_parse_int();
  assert(t == TLS_COMBINATOR);

  tl_combinator *combinator = new tl_combinator();
  combinator->id = try_parse_int();
  combinator->name = try_parse_string();
  combinator->type_id = try_parse_int();
  combinator->var_count = 0;

  int left_type = try_parse_int();
  if (left_type == TLS_COMBINATOR_LEFT) {
    combinator->args = read_args_list(&combinator->var_count);
  } else {
    assert(left_type == TLS_COMBINATOR_LEFT_BUILTIN);
  }

  int32_t right_ver = try_parse_int();
  assert(right_ver == TLS_COMBINATOR_RIGHT_V2);
  combinator->result = read_type_expr(&combinator->var_count);

  return combinator;
}

tl_type *tl_config_parser::read_type(void) {
  int32_t t = try_parse_int();
  assert(t == TLS_TYPE);

  tl_type *type = new tl_type();
  type->id = try_parse_int();
  type->name = try_parse_string();
  type->constructors_num = try_parse_int();
  type->constructors.reserve(type->constructors_num);
  type->flags = try_parse_int();
  type->flags &= ~(1 | 8 | 16 | 1024);
  if (type->flags != 0) {
    fprintf(stderr, "Type %s has non-zero flags: %d\n", type->name.c_str(), type->flags);
  }
  type->arity = try_parse_int();

  try_parse_long();  // unused
  return type;
}

tl_config tl_config_parser::parse_config(void) {
  schema_version = get_schema_version(try_parse_int());
  assert(schema_version >= 2);

  try_parse_int();  // date
  try_parse_int();  // version

  int types_n = try_parse_int();
  int constructors_total = 0;
  for (int i = 0; i < types_n; i++) {
    tl_type *type = read_type();
    config.add_type(type);
    constructors_total += type->constructors_num;
  }

  int constructors_n = try_parse_int();
  assert(constructors_n == constructors_total);
  for (int i = 0; i < constructors_n; i++) {
    tl_combinator *constructor = read_combinator();
    config.get_type(constructor->type_id)->add_constructor(constructor);
  }

  int functions_n = try_parse_int();
  for (int i = 0; i < functions_n; i++) {
    config.add_function(read_combinator());
  }
  p.fetch_end();
  try_parse(0);

  return config;
}

}  // namespace tl
}  // namespace td
