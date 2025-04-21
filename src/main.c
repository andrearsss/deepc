#include <stdio.h>
#include "matrix.h"

int main() {
    //int * x;
    //prova(x);
    //printf("%d\n", *x);

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

    printf("\nProduct");
    mat_print(matrix);
    printf("*");
    mat_print(matrix2);

    if ((ret = mat_mul_ew(matrix, matrix2)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }

    printf("=");
    mat_print(matrix); 

    mat_destroy(matrix);
    mat_destroy(matrix2);
    printf("\n");
    return 0;
}

