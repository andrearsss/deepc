#include <stdio.h>
#include "matrix.h"
#include <cblas.h>

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
    //test1();
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
    //mat_print(matrix);
    //mat_print(matrix2);

    printf("\n");
    
    if ((ret = mat_dot(matrix, matrix2, &matrix_dot)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }
    mat_print(matrix_dot);

    printf("\n");


    // comparison using BLAS
    
    // cast to double
    double A[m*k];
    double B[k*n];
    for (int i = 0; i < m*k; ++i) {
        A[i] = (double)A_f[i];
    }
    for (int i = 0; i < k*n; ++i) {
        B[i] = (double)B_f[i];
    }

    // Leading dimensions
    int lda = 2;
    int ldb = 3; 
    int ldc = 3;

    double C[m * n];
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                m, n, k, 1.0, A, lda, B, ldb, 0.0, C, ldc);

    printf("BLAS\n");
    for (int i = 0; i < m*n; i++) {
        printf("%8.2f%s", C[i], (((i+1) % (n)) == 0) ? "\n" : " ");
    }

    // check correctness
    int correct = 1;
    float value;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if ((ret = mat_get(matrix_dot, i, j, &value)) != MAT_SUCCESS) {
                mat_print_error(ret);
                return 0;
            }
            if (C[i * n + j] != value) {
                printf("\nError at %d, %d", i, j);
                printf("\nValue: %.2f", value);
                correct = 0;
                return 0;
            }
        }
    }

    if (!correct)
        printf("\nTEST FAILED\n");
    else
        printf("\nSUCCESS\n");

    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(matrix_dot);
    return 0;
}

