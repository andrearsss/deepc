#include "error.h"

const char * error_string(RET err) {
    switch (err) {
        case SUCCESS: return "Success";
        case NULL_POINTER: return "Null pointer";
        case ALLOC_FAILED: return "Allocation failed";        
        case INVALID_DIM: return "Invalid dimension(s)";
        case INVALID_SHAPE: return "Invalid shape";
        default: return "Unknown error";
    }
}