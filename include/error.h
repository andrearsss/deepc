#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum {
    SUCCESS = 0,
    NULL_POINTER,
    ALLOC_FAILED,
    INVALID_DIM,
    INVALID_SHAPE,
    INVALID_ACTIVATION,
    NUMERICAL_ERROR     // nan, inf...
} RET;

const char * error_string(RET err);

#define print_error(x) do { RET e = (x); if (e != SUCCESS) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, error_string(e)); \
    return e; }} while(0)

#endif