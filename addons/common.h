#include <jerryscript.h>

#pragma once

#define JERRY_DECL_FUNC(name)                                                  \
    jerry_value_t name(const jerry_value_t function_obj,                     \
                       const jerry_value_t this, const jerry_value_t argv[], \
                       const jerry_length_t argc)


#define JERRY_DECL_FUNC_ARGS(name, ...)                                        \
    jerry_value_t name(const jerry_value_t function_obj,                     \
                       const jerry_value_t this, const jerry_value_t argv[], \
                       const jerry_length_t argc, __VA_ARGS__)

#define JERRY_CHAIN_FUNC_ARGS(name, ...) \
    name(function_obj, this, argv, argc, __VA_ARGS__)

void jerry_add_func_to_obj(jerry_value_t obj, const char *name, void *func);
void jerry_add_obj_to_obj(jerry_value_t parent, const char *name, jerry_value_t child);
void jerry_set_named_property(jerry_value_t obj, const char *name, jerry_value_t prop);