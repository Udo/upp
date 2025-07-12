/*
 * UPP Prepass - Source File Directive Parser
 * 
 * This module parses UPP-specific directives from source files and
 * stores them in a hashmap for use by the compiler.
 * 
 * Supported directives:
 *   #param key "value"      - Set a parameter
 *   #param key value        - Set a parameter (unquoted)
 *   #binary exe "path"      - Set binary output path
 *   #binary exe path        - Set binary output path (unquoted)
 * 
 * Copyright (c) 2025 Udo Schroeter
 */

#ifndef UPP_PREPASS_H
#define UPP_PREPASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#ifdef TCC_TARGET_X86_64
#include "../tcc.h"
#define UPP_MALLOC(s) tcc_malloc(s)
#define UPP_FREE(p) tcc_free(p)
#define UPP_STRDUP(s) tcc_strdup(s)
#else
#define UPP_MALLOC(s) malloc(s)
#define UPP_FREE(p) free(p)
#define UPP_STRDUP(s) strdup(s)
#endif

#include "hash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UPP_PREPASS_MAX_LINE 4096

#define UPP_PREPASS_MAX_TOKEN 1024

typedef struct upp_prepass_result {
    upp_hash_t *parameters;     /* Parameter key-value pairs */
    char *binary_path;          /* Binary output path if specified */
    int directive_count;        /* Number of directives processed */
    int error_count;           /* Number of parse errors */
    char *last_error;          /* Last error message */
    char *cmdline_options;     /* Accumulated command-line options */
} upp_prepass_result_t;

typedef enum upp_prepass_error {
    UPP_PREPASS_SUCCESS = 0,
    UPP_PREPASS_ERROR_FILE_NOT_FOUND,
    UPP_PREPASS_ERROR_MEMORY,
    UPP_PREPASS_ERROR_INVALID_DIRECTIVE,
    UPP_PREPASS_ERROR_PARSE_ERROR
} upp_prepass_error_t;

static inline upp_prepass_result_t *upp_prepass_result_create(void) {
    upp_prepass_result_t *result;
    result = (upp_prepass_result_t*)UPP_MALLOC(sizeof(upp_prepass_result_t));
    if (!result) return NULL;
    
    result->parameters = upp_hash_create();
    if (!result->parameters) {
        UPP_FREE(result);
        return NULL;
    }
    
    result->binary_path = NULL;
    result->directive_count = 0;
    result->error_count = 0;
    result->last_error = NULL;
    result->cmdline_options = NULL;
    
    return result;
}

static inline void upp_prepass_result_destroy(upp_prepass_result_t *result) {
    if (!result) return;
    
    if (result->parameters) {
        upp_hash_destroy(result->parameters);
    }
    
    UPP_FREE(result->binary_path);
    UPP_FREE(result->last_error);
    UPP_FREE(result->cmdline_options);
    UPP_FREE(result);
}

static inline void upp_prepass_set_error(upp_prepass_result_t *result, const char *error) {
    if (!result || !error) return;
    
    UPP_FREE(result->last_error);
    result->last_error = UPP_STRDUP(error);
    result->error_count++;
}

/* Check if a parameter key corresponds to a command-line option */
static inline bool upp_prepass_is_cmdline_option(const char *key) {
    /* Common TCC command-line options that can be set via UPP parameters */
    static const char *cmdline_options[] = {
        "O", "O0", "O1", "O2", "O3",          /* Optimization levels */
        "g", "gdwarf-2", "gdwarf-3", "gdwarf-4", /* Debug options */
        "w",                                   /* Disable warnings */
        "v", "vv",                            /* Verbose options */
        "bench",                              /* Benchmark mode */
        "static",                             /* Static linking */
        "shared",                             /* Shared library */
        "rdynamic",                           /* Export symbols */
        "c",                                  /* Compile only */
        "E",                                  /* Preprocess only */
        "r",                                  /* Relocatable object */
        "nostdinc",                           /* No standard includes */
        "nostdlib",                           /* No standard libraries */
        NULL
    };
    
    const char **opt = cmdline_options;
    while (*opt) {
        if (strcmp(key, *opt) == 0) {
            return true;
        }
        opt++;
    }
    
    /* Check for -D, -U, -I, -L, -l prefixes */
    if (key[0] == 'D' || key[0] == 'U' || key[0] == 'I' || 
        key[0] == 'L' || key[0] == 'l') {
        return true;
    }
    
    /* Check for -f and -W options */
    if (strlen(key) > 1 && (key[0] == 'f' || key[0] == 'W')) {
        return true;
    }
    
    return false;
}

/* Apply UPP command-line options directly to TCCState */
static inline void upp_prepass_apply_options(const upp_prepass_result_t *result, void *tcc_state) {
    upp_hash_iterator_t iter;
    
    if (!result || !result->parameters || !tcc_state) return;
    
    /* We need to include tcc.h to access TCCState, so let's pass it as void* and cast */
    TCCState *s = (TCCState*)tcc_state;
    
    /* Iterate through parameters and apply known options directly */
    iter = upp_hash_iterator_init(result->parameters);
    while (upp_hash_iterator_has_next(&iter)) {
        upp_hash_entry_t *entry = upp_hash_iterator_next(&iter);
        const char *key = entry->key;
        const char *value = entry->value;
        
        /* Handle direct compiler state changes for critical options */
        if (strcmp(key, "g") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
                s->do_debug = 1;
            }
        } else if (strcmp(key, "w") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
                s->warn_none = 1;
            }
        } else if (strcmp(key, "v") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
                s->verbose = 1;
            }
        } else if (strcmp(key, "vv") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
                s->verbose = 2;
            }
        } else if (key[0] == 'D' && strlen(key) > 1) {
            /* Handle -D defines using tcc_set_options instead of tcc_define_symbol */
            char option_str[1024];
            const char *symbol_name = key + 1;  /* Skip the 'D' */
            if (strlen(value) > 0) {
                snprintf(option_str, sizeof(option_str), "-D%s=%s", symbol_name, value);
            } else {
                snprintf(option_str, sizeof(option_str), "-D%s", symbol_name);
            }
            tcc_set_options(s, option_str);
        } else if (key[0] == 'U' && strlen(key) > 1) {
            /* Handle -U undefines */
            tcc_undefine_symbol(s, key + 1);
        } else if (key[0] == 'I' && strlen(key) > 1) {
            /* Handle -I include paths */
            tcc_add_include_path(s, key + 1);
        } else if (key[0] == 'L' && strlen(key) > 1) {
            /* Handle -L library paths */
            tcc_add_library_path(s, key + 1);
        } else if (key[0] == 'l' && strlen(key) > 1) {
            /* Handle -l libraries */
            tcc_add_library(s, key + 1);
        }
    }
}

static inline const char *upp_prepass_skip_whitespace(const char *str) {
    while (*str && isspace(*str)) str++;
    return str;
}

static inline const char *upp_prepass_parse_quoted_string(const char *str, char *output, size_t max_len) {
    size_t i;
    
    if (!str || *str != '"') return NULL;
    
    str++; /* skip opening quote */
    i = 0;
    
    while (*str && *str != '"' && i < max_len - 1) {
        if (*str == '\\' && str[1]) {
            str++; /* skip backslash */
            switch (*str) {
                case 'n': output[i++] = '\n'; break;
                case 't': output[i++] = '\t'; break;
                case 'r': output[i++] = '\r'; break;
                case '\\': output[i++] = '\\'; break;
                case '"': output[i++] = '"'; break;
                default: output[i++] = *str; break;
            }
        } else {
            output[i++] = *str;
        }
        str++;
    }
    
    if (*str == '"') {
        str++; /* skip closing quote */
    }
    
    output[i] = '\0';
    return str;
}

static inline const char *upp_prepass_parse_token(const char *str, char *output, size_t max_len) {
    size_t i = 0;
    
    while (*str && !isspace(*str) && i < max_len - 1) {
        output[i++] = *str++;
    }
    
    output[i] = '\0';
    return str;
}

static inline bool upp_prepass_parse_param(const char *line, upp_prepass_result_t *result) {
    char key[UPP_PREPASS_MAX_TOKEN];
    char value[UPP_PREPASS_MAX_TOKEN];
    
    line = upp_prepass_skip_whitespace(line + 6);
    
    line = upp_prepass_parse_token(line, key, sizeof(key));
    if (!key[0]) {
        upp_prepass_set_error(result, "Missing parameter key");
        return false;
    }
    
    line = upp_prepass_skip_whitespace(line);
    
    if (*line == '"') {
        line = upp_prepass_parse_quoted_string(line, value, sizeof(value));
        if (!line) {
            upp_prepass_set_error(result, "Unterminated quoted string");
            return false;
        }
    } else {
        line = upp_prepass_parse_token(line, value, sizeof(value));
    }
    
    if (!value[0]) {
        upp_prepass_set_error(result, "Missing parameter value");
        return false;
    }
    
    /* Store in hashmap */
    if (!upp_hash_put(result->parameters, key, value)) {
        upp_prepass_set_error(result, "Failed to store parameter");
        return false;
    }
    
    return true;
}

static inline bool upp_prepass_parse_binary(const char *line, upp_prepass_result_t *result) {
    char path[UPP_PREPASS_MAX_TOKEN];
    
    line = upp_prepass_skip_whitespace(line + 7);
    
    if (strncmp(line, "exe", 3) == 0 && (isspace(line[3]) || line[3] == '\0')) {
        line = upp_prepass_skip_whitespace(line + 3);
    }
    
    /* Parse path */
    if (*line == '"') {
        line = upp_prepass_parse_quoted_string(line, path, sizeof(path));
        if (!line) {
            upp_prepass_set_error(result, "Unterminated quoted string in binary path");
            return false;
        }
    } else {
        line = upp_prepass_parse_token(line, path, sizeof(path));
    }
    
    if (!path[0]) {
        upp_prepass_set_error(result, "Missing binary path");
        return false;
    }
    
    /* Store binary path */
    UPP_FREE(result->binary_path);
    result->binary_path = UPP_STRDUP(path);
    if (!result->binary_path) {
        upp_prepass_set_error(result, "Failed to store binary path");
        return false;
    }
    
    return true;
}

/* Parse a single line for directives */
static inline bool upp_prepass_parse_line(const char *line, upp_prepass_result_t *result) {
    /* Skip leading whitespace */
    line = upp_prepass_skip_whitespace(line);
    
    /* Check if line starts with # */
    if (*line != '#') {
        return true; /* Not a directive, ignore */
    }
    
    /* Check for supported directives */
    if (strncmp(line, "#param", 6) == 0 && (isspace(line[6]) || line[6] == '\0')) {
        if (upp_prepass_parse_param(line, result)) {
            result->directive_count++;
            return true;
        }
        return false;
    }
    
    if (strncmp(line, "#binary", 7) == 0 && (isspace(line[7]) || line[7] == '\0')) {
        if (upp_prepass_parse_binary(line, result)) {
            result->directive_count++;
            return true;
        }
        return false;
    }
    
    return true;
}

static inline upp_prepass_result_t *upp_prepass_parse_file(const char *filename) {
    FILE *file;
    upp_prepass_result_t *result;
    char line[UPP_PREPASS_MAX_LINE];
    int line_number;
    
    if (!filename) return NULL;
    
    file = fopen(filename, "r");
    if (!file) {
        result = upp_prepass_result_create();
        if (result) {
            upp_prepass_set_error(result, "Failed to open file");
        }
        return result;
    }
    
    result = upp_prepass_result_create();
    if (!result) {
        fclose(file);
        return NULL;
    }
    
    line_number = 0;
    
    while (fgets(line, sizeof(line), file)) {
        size_t len;
        line_number++;
        
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        if (!upp_prepass_parse_line(line, result)) {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Parse error at line %d: %s", 
                     line_number, result->last_error ? result->last_error : "Unknown error");
            upp_prepass_set_error(result, error_msg);
        }
    }
    
    fclose(file);
    return result;
}

static inline upp_prepass_result_t *upp_prepass_parse_string(const char *source) {
    upp_prepass_result_t *result;
    char line[UPP_PREPASS_MAX_LINE];
    const char *ptr;
    int line_number;
    
    if (!source) return NULL;
    
    result = upp_prepass_result_create();
    if (!result) return NULL;
    
    ptr = source;
    line_number = 0;
    
    while (*ptr) {
        size_t i;
        line_number++;
        
        i = 0;
        while (*ptr && *ptr != '\n' && i < sizeof(line) - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        
        if (*ptr == '\n') ptr++;
        
        if (!upp_prepass_parse_line(line, result)) {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Parse error at line %d: %s", 
                     line_number, result->last_error ? result->last_error : "Unknown error");
            upp_prepass_set_error(result, error_msg);
        }
    }
    
    return result;
}

static inline const char *upp_prepass_get_param(const upp_prepass_result_t *result, const char *key) {
    if (!result || !result->parameters || !key) return NULL;
    return upp_hash_get(result->parameters, key);
}

static inline bool upp_prepass_has_param(const upp_prepass_result_t *result, const char *key) {
    return upp_prepass_get_param(result, key) != NULL;
}

static inline const char *upp_prepass_get_binary_path(const upp_prepass_result_t *result) {
    return result ? result->binary_path : NULL;
}

static inline int upp_prepass_get_directive_count(const upp_prepass_result_t *result) {
    return result ? result->directive_count : 0;
}

static inline int upp_prepass_get_error_count(const upp_prepass_result_t *result) {
    return result ? result->error_count : 0;
}

static inline const char *upp_prepass_get_last_error(const upp_prepass_result_t *result) {
    return result ? result->last_error : NULL;
}

static inline const char *upp_prepass_get_cmdline_options(const upp_prepass_result_t *result) {
    return result ? result->cmdline_options : NULL;
}

static inline void upp_prepass_print_params(const upp_prepass_result_t *result) {
    upp_hash_iterator_t iter;
    
    if (!result || !result->parameters) {
        printf("Prepass result: NULL\n");
        return;
    }
    
    printf("UPP Prepass Results:\n");
    printf("  Directives processed: %d\n", result->directive_count);
    printf("  Parse errors: %d\n", result->error_count);
    
    if (result->binary_path) {
        printf("  Binary path: %s\n", result->binary_path);
    }
    
    if (result->last_error) {
        printf("  Last error: %s\n", result->last_error);
    }
    
    printf("  Parameters:\n");
    
    iter = upp_hash_iterator_init(result->parameters);
    while (upp_hash_iterator_has_next(&iter)) {
        upp_hash_entry_t *entry = upp_hash_iterator_next(&iter);
        printf("    %s = %s\n", entry->key, entry->value);
    }
    
    if (upp_hash_size(result->parameters) == 0) {
        printf("    (no parameters)\n");
    }
}

#ifdef __cplusplus
}
#endif

#endif /* UPP_PREPASS_H */
