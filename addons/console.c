#include <jerryscript.h>
#include "common.h"

#define MAX_STR_LENGTH 256

#define STDERR_PRINT(s, ...) fprintf(stderr, s, __VA_ARGS__)
#define STDOUT_PRINT(s, ...) fprintf(stdout, s, __VA_ARGS__)
#define IS_NUMBER 0
#define IS_INT    1
#define IS_UINT   2

static jerry_value_t global_console_obj;

static int is_int(jerry_value_t val)
{
    int ret = 0;
    double n = jerry_get_number_value(val);
    ret = (n - (int)n == 0);
    if (ret) {
        // Integer type
        if (n < 0) {
            return IS_INT;
        } else {
            return IS_UINT;
        }
    } else {
        return IS_NUMBER;
    }
}

static bool value2str(const jerry_value_t value, char *buf, int maxlen,
                      bool quotes)
{
    // requires: buf has at least maxlen characters
    //  effects: writes a string representation of the value to buf; when
    //             processing a string value, puts quotes around it if quotes
    //             is true
    //  returns: true if the representation was complete or false if it
    //             was abbreviated
    jerry_value_t str_val = jerry_create_undefined();
    bool is_string = false;

    jerry_foreach_object_property()
    if (jerry_value_is_array(value)) {
        uint32_t len = jerry_get_array_length(value);
        sprintf(buf, "[Array - length %u]", len);
        return false;
    } else if (jerry_value_is_boolean(value)) {
        uint8_t val = jerry_get_boolean_value(value);
        sprintf(buf, (val) ? "true" : "false");
    } else if (jerry_value_is_function(value)) {
        sprintf(buf, "[Function]");
    } else if (jerry_value_is_number(value)) {
        int type = is_int(value);
        if (type == IS_NUMBER) {
            str_val = jerry_value_to_string(value);
            is_string = true;
        } else if (type == IS_UINT) {
            unsigned int num = jerry_get_number_value(value);
            sprintf(buf, "%u", num);
        } else if (type == IS_INT) {
            int num = jerry_get_number_value(value);
            sprintf(buf, "%d", num);
        }
    } else if (jerry_value_is_null(value)) {
        sprintf(buf, "null");
    }
        // NOTE: important that checks for function and array were above this
    else if (jerry_value_is_object(value)) {
        sprintf(buf, "[Object]");
    } else if (jerry_value_is_string(value)) {
        str_val = jerry_value_to_string(value);
        is_string = true;
    } else if (jerry_value_is_undefined(value)) {
        sprintf(buf, "undefined");
    } else {
        // should never get this
        sprintf(buf, "UNKNOWN");
    }

    if (is_string) {
        jerry_size_t size = jerry_get_string_size(str_val);
        if (size >= maxlen) {
            sprintf(buf, "[String - length %u]", (unsigned int)size);
        } else {
            char buffer[++size];
            jerry_size_t str_size = jerry_get_string_size(str_val);
            jerry_size_t len = 0;
            if (maxlen > str_size)
                len = jerry_string_to_utf8_char_buffer(str_val, (jerry_char_t *)buffer,
                                                       size);
            buffer[len] = '\0';
            if (quotes) {
                sprintf(buf, "\"%s\"", buffer);
            } else {
                sprintf(buf, "%s", buffer);
            }
        }
    }
    return true;
}

static void print_value(const jerry_value_t value, FILE *out, bool deep, bool quotes)
{
    char buf[MAX_STR_LENGTH];
    if (!value2str(value, buf, MAX_STR_LENGTH, quotes) && deep) {
        if (jerry_value_is_array(value)) {
            uint32_t len = jerry_get_array_length(value);
#if (JERRY_DEBUGGER == 1)
            jerry_debugger_send_output((jerry_char_t *)"[", 1, JERRY_DEBUGGER_OUTPUT_OK);
#endif
            fprintf(out, "[");
            for (int i = 0; i < len; i++) {
                if (i) {
#if (JERRY_DEBUGGER == 1)
                    jerry_debugger_send_output((jerry_char_t *)", ", 2, JERRY_DEBUGGER_OUTPUT_OK);
#endif
                    fprintf(out, ", ");
                }
                jerry_value_t element = jerry_get_property_by_index(value, i);
                print_value(element, out, false, true);
            }
#if (JERRY_DEBUGGER == 1)
            jerry_debugger_send_output((jerry_char_t *)"]", 1, JERRY_DEBUGGER_OUTPUT_OK);
#endif
            fprintf(out, "]");
        }
    } else {
#if (JERRY_DEBUGGER == 1)
        jerry_debugger_send_output((jerry_char_t *)buf, strlen(buf), JERRY_DEBUGGER_OUTPUT_OK);
#endif
        fprintf(out, "%s", buf);
    }
}

static JERRY_DECL_FUNC_ARGS(do_print, FILE *out)
{
    for (int i = 0; i < argc; i++) {
        if (i) {
        // insert spaces between arguments
#if (JERRY_DEBUGGER == 1)
            jerry_debugger_send_output((jerry_char_t *)" ", 1, JERRY_DEBUGGER_OUTPUT_OK);
#endif
            fprintf(out, " ");
        }
        print_value(argv[i], out, true, false);
    }
    fprintf(out, "\n");
    return jerry_create_undefined();
}

static JERRY_DECL_FUNC(console_log)
{
    return JERRY_CHAIN_FUNC_ARGS(do_print, stdout);
}

static JERRY_DECL_FUNC(console_error)
{
    return JERRY_CHAIN_FUNC_ARGS(do_print, stderr);
}

void jerry_esp_console_init()
{
    jerry_value_t console = jerry_create_object();
    jerry_add_func_to_obj(console, "log", console_log);
    jerry_add_func_to_obj(console, "info", console_log);
    jerry_add_func_to_obj(console, "error", console_error);
    jerry_add_func_to_obj(console, "warn", console_error);

    jerry_value_t global_obj = jerry_get_global_object();
    jerry_set_named_property(global_obj, "console", console);

    // initialize the time object
    global_console_obj = jerry_create_object();
}

void jerry_esp_console_deinit()
{
    jerry_release_value(global_console_obj);
}