#include <stdio.h>
#include "matrix.h"

int test1() {
    Matrix * matrix;
    MAT_RET ret;
    const float data[] = { 1, 2, 3.110454,
                     4, 5, 6,
                     7, 8, 9};
    
    if ((ret = mat_create(data, 3, 3, &matrix)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    
    mat_print(matrix);
    printf("+5");
    mat_add_bias(matrix, 5);
    mat_print(matrix);

    Matrix * matrix2;
    if ((ret = mat_create(data, 3, 3, &matrix2)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }

    printf("\nSum");
    mat_print(matrix);
    printf("+");
    mat_print(matrix2);

    if ((ret = mat_add_ew(matrix, matrix2)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }

    printf("=");
    mat_print(matrix);

    printf("\nElement-wise product");
    mat_print(matrix);
    printf("*");
    mat_print(matrix2);

    if ((ret = mat_mul_ew(matrix, matrix2)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }

    printf("=");
    mat_print(matrix); 

    printf("\nTranspose:");
    if ((ret = mat_transpose(matrix)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix); 

    printf("\nCopy:");
    Matrix * matrix_copy;
    if ((ret = mat_copy(matrix, &matrix_copy)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix_copy);
    
    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(matrix_copy);
    return 0;
}

int main() {
    Matrix * matrix;
    Matrix * matrix2;
    Matrix * matrix_dot;
    MAT_RET ret;

    const float vec1[] = { 1, 2, 3, 4};
    const float vec2[] = { 3, 10};

    if ((ret = mat_create(vec1, 2, 2, &matrix)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    if ((ret = mat_create(vec2, 2, 1, &matrix2)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix);
    mat_print(matrix2);

    printf("\nDot product:");
    
    if ((ret = mat_dot(matrix, matrix2, &matrix_dot)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix_dot);

    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(matrix_dot);
    printf("\n");
    return 0;
}

