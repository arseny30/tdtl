#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>

#include "tl_core.h"
#include "td/utils/tl_parser.h"

namespace td {
namespace tl {

class tl_config {
  std::vector<tl_type *> types;
  std::map<int, tl_type *> id_to_type;
  std::map<std::string, tl_type *> name_to_type;

  std::vector<tl_combinator *> functions;
  std::map<int, tl_combinator *> id_to_function;
  std::map<std::string, tl_combinator *> name_to_function;

 public:
  void add_type(tl_type *type);

  tl_type *get_type(int type_id);

  tl_type *get_type(const std::string &type_name);

  void add_function(tl_combinator *function);

  tl_combinator *get_function(int function_id);

  tl_combinator *get_function(const std::string &function_name);

  int get_type_count(void) const;

  tl_type *get_type_by_num(int num) const;

  int get_function_count(void) const;

  tl_combinator *get_function_by_num(int num) const;
};

class tl_config_parser {
  tl_parser p;
  int schema_version;
  tl_config config;

  static int get_schema_version(int a);

  tl_tree *read_num_const(void);
  tl_tree *read_num_var(int *var_count);
  tl_tree *read_type_var(int *var_count);
  tl_tree *read_array(int *var_count);
  tl_tree *read_type(int *var_count);
  tl_tree *read_type_expr(int *var_count);
  tl_tree *read_nat_expr(int *var_count);
  tl_tree *read_expr(int *var_count);
  std::vector<arg> read_args_list(int *var_count);

  tl_combinator *read_combinator(void);
  tl_type *read_type(void);

  template <class T>
  T try_parse(const T &res) const;

  int32_t try_parse_int(void);
  int64_t try_parse_long(void);
  std::string try_parse_string(void);

 public:
  tl_config_parser(const int32_t *s, int len) : p(s, len) {
  }

  tl_config parse_config(void);
};

}  // namespace tl
}  // namespace td
