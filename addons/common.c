#include <jerryscript.h>
#include "common.h"

void jerry_add_func_to_obj(jerry_value_t obj, const char *name, void *func)
{
    jerry_value_t jname = jerry_create_string((const jerry_char_t *)name);
    jerry_value_t jfunc = jerry_create_external_function(func);
    if (jerry_value_is_function(jfunc)) {
        jerry_set_property(obj, jname, jfunc);
    }
}

void jerry_add_obj_to_obj(jerry_value_t parent, const char *name, jerry_value_t child)
{
    jerry_value_t jname = jerry_create_string((const jerry_char_t *)name);
    jerry_set_property(parent, jname, child);
}

void jerry_set_named_property(const jerry_value_t obj, const char *name, const jerry_value_t prop)
{
    jerry_value_t jname = jerry_create_string((const jerry_char_t *)name);
    jerry_set_property(obj, jname, prop);
}
