#include <stdio.h>
#include "matrix.h"
#include <cblas.h>



int main() {
    Matrix * matrix;
    Matrix * matrix2;
    Matrix * matrix_dot;
    MAT_RET ret;

    float A_f[6] = {
        1.0f, 2.0f,
        3.0f, 4.0f,
        5.0f, 6.0f
    };

    float B_f[6] = {
        7.0f,  8.0f,  9.0f,
        10.0f, 11.0f, 12.0f
    };

    // A = m*k, B = k*n
    int m = 3;
    int n = 3;
    int k = 2;

    if ((ret = mat_create(A_f, m, k, &matrix)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    if ((ret = mat_create(B_f, k, n, &matrix2)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix);
    mat_print(matrix2);
    printf("\n");
    
    if ((ret = mat_dot(matrix, matrix2, &matrix_dot)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix_dot);
    printf("\n");

    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(matrix_dot);
    return 0;
}

