#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef struct matrix Matrix;

typedef enum {
    MAT_SUCCESS = 0,
    MAT_NULL_POINTER,
    MAT_ALLOC_FAILED,
    MAT_INVALID_DIM,
    MAT_INVALID_SHAPE
} MAT_RET;

MAT_RET mat_create(const float * data, int n_rows, int n_cols, Matrix ** m);
MAT_RET mat_copy(const Matrix * m, Matrix ** m_copy);
MAT_RET mat_get(const Matrix * m, int i, int j, float * out);
MAT_RET mat_transpose(Matrix * m);                                        // in-place transpose
MAT_RET mat_add_bias(Matrix * m, int bias);                               // add bias
MAT_RET mat_add_ew(Matrix * m1, const Matrix * m2);                       // element-wise sum
MAT_RET mat_mul_ew(Matrix * m1, const Matrix * m2);                       // element-wise product
MAT_RET mat_dot(Matrix * m1, const Matrix * m2, Matrix ** m);             // dot product
  
void mat_print(const Matrix * m);
void mat_destroy(Matrix * m);     

const char * mat_error_string(MAT_RET err);

#define mat_print_error(x) do { MAT_RET e = (x); if (e != MAT_SUCCESS) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, mat_error_string(e)); \
    return e; }} while(0)

#endif