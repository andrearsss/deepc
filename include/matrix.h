#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "error.h"

enum {
    NO_INIT = 0,
    ZERO_INIT,
    HE_INIT
};

typedef struct matrix Matrix;

RET mat_create(const float * data, int n_rows, int n_cols, int init, Matrix ** m);
RET mat_copy(const Matrix * m, Matrix ** m_copy);
RET mat_get(const Matrix * m, int i, int j, float * out);
RET mat_transpose(Matrix * m);                                                                  // in-place transpose
RET mat_add_bias(Matrix * m, int bias);                                                         // add bias
RET mat_add_ew(Matrix * m1, const Matrix * m2);                                                 // element-wise sum
RET mat_mul_ew(Matrix * m1, const Matrix * m2);                                                 // element-wise product
RET mat_dot(Matrix * m1, const Matrix * m2, Matrix ** m);                                       // dot product
RET mat_linear(const Matrix * m1, const Matrix * m2_T, const Matrix * bias, Matrix ** m_out);   // m1*m2 + b
RET mat_apply(Matrix * m, float (*act)(float));                                                 // apply a func to all elements

void mat_print(const Matrix * m);
void mat_destroy(Matrix * m);     

#endif