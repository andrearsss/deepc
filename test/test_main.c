#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cblas.h>

#include "matrix.h"
#include "tests.h"

int test1() {
    Matrix * matrix;
    RET ret;
    const float data[] = { 1, 2, 3.110454,
                     4, 5, 6,
                     7, 8, 9};
    
    if ((ret = mat_create(data, 3, 3, &matrix)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    
    //mat_print(matrix);
    //printf("+5");
    //mat_add_bias(matrix, 5);
    //mat_print(matrix);

    Matrix * matrix2;
    if ((ret = mat_create(data, 3, 3, &matrix2)) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    //printf("\nSum");
    //mat_print(matrix);
    //printf("+");
    //mat_print(matrix2);

    if ((ret = mat_add_ew(matrix, matrix2)) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    //printf("=");
    //mat_print(matrix);

    //printf("\nElement-wise product");
    //mat_print(matrix);
    //printf("*");
    //mat_print(matrix2);

    if ((ret = mat_mul_ew(matrix, matrix2)) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    //printf("=");
    //mat_print(matrix); 

    //printf("\nTranspose:");
    if ((ret = mat_transpose(matrix)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    //mat_print(matrix); 

    //printf("\nCopy:");
    Matrix * matrix_copy;
    if ((ret = mat_copy(matrix, &matrix_copy)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    //mat_print(matrix_copy);
    
    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(matrix_copy);
    return 0;
}

int test_dot() {
    RET ret;
    Matrix *matrix;
    Matrix *matrix2;
    Matrix *matrix_dot;
    double *A;
    double *B;
    double *C;

    // Array of test cases
    TestCase tests[] = {
        {"Original 3x2 * 2x3", A1, B1, 3, 3, 2},
        {"Square 2x2 * 2x2", A2, B2, 2, 2, 2},
        {"Rectangle 2x3 * 3x1", A3, B3, 2, 1, 3},
        {"Zero Matrix 2x2 * 2x2", A4, B4, 2, 2, 2},
        {"Rectangle 1x4 * 4x2", A5, B5, 1, 2, 4},
        {"Identity 3x3 * Identity 3x3", A6, B6, 3, 3, 3},
        {"Non-Square 4x2 * 2x3", A7, B7, 4, 3, 2},
        {"Large Diagonal 5x5 * Identity 5x5", A8, B8, 5, 5, 5}
    };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int overall_success = 1;

    printf("Starting Matrix Dot Test...\n");
    printf("===============================\n\n");

    for (int t = 0; t < num_tests; ++t) {
        TestCase currentTest = tests[t];
        printf("--- Test Case %d: %s ---\n", t + 1, currentTest.name);
        printf("Dimensions: A(%d x %d), B(%d x %d)\n",
               currentTest.m, currentTest.k, currentTest.k, currentTest.n);
        
        int test_passed = 1;

        if ((ret = mat_create(currentTest.A, currentTest.m, currentTest.k, &matrix)) != SUCCESS) {
            fprintf(stderr, "Test %d FAILED: mat_create (A) error: ", t + 1);
            print_error(ret);
            test_passed = 0;
            goto cleanup;
        }
        if ((ret = mat_create(currentTest.B, currentTest.k, currentTest.n, &matrix2)) != SUCCESS) {
            fprintf(stderr, "Test %d FAILED: mat_create (B) error: ", t + 1);
            print_error(ret);
            test_passed = 0;
            goto cleanup;
        }

        printf("\nmat_dot Result:\n");
        if ((ret = mat_dot(matrix, matrix2, &matrix_dot)) != SUCCESS) {
            fprintf(stderr, "Test %d FAILED: mat_dot error: ", t + 1);
            print_error(ret);
            test_passed = 0;
            goto cleanup;
        }
        mat_print(matrix_dot);
        printf("\n");

        // --- BLAS Comparison ---

        int size_A = currentTest.m * currentTest.k;
        int size_B = currentTest.k * currentTest.n;
        int size_C = currentTest.m * currentTest.n;

        A = (double*)malloc(size_A * sizeof(double));
        B = (double*)malloc(size_B * sizeof(double));
        C = (double*)malloc(size_C * sizeof(double));

        if (!A || !B || !C) {
             fprintf(stderr, "Test %d FAILED: Memory allocation error for BLAS arrays.\n", t+1);
             test_passed = 0;
             goto cleanup;
        }

        // Cast float input arrays to double for BLAS
        for (int i = 0; i < size_A; ++i) {
            A[i] = (double)currentTest.A[i];
        }
        for (int i = 0; i < size_B; ++i) {
            B[i] = (double)currentTest.B[i];
        }

        // Leading dimensions for RowMajor order
        // lda = number of columns in A = k
        // ldb = number of columns in B = n
        // ldc = number of columns in C = n
        int lda = currentTest.k;
        int ldb = currentTest.n;
        int ldc = currentTest.n;

        // Perform matrix multiplication using BLAS dgemm
        // C = alpha*A*B + beta*C
        // Here: C = 1.0 * A * B + 0.0 * C
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                    currentTest.m, currentTest.n, currentTest.k,
                    1.0, A, lda, B, ldb, 0.0, C, ldc);

        printf("BLAS Result (cblas_dgemm):\n");
        for (int i = 0; i < currentTest.m; i++) {
            for (int j = 0; j < currentTest.n; j++) {
                 printf("%8.2f%s", C[i * ldc + j], (j == currentTest.n - 1) ? "" : " ");
            }
             printf("\n");
        }
        printf("\n");

        // --- Check Correctness ---
        // Compare results element by element with tolerance
        float value_custom;
        double value_blas;
        float tolerance = 1e-6f;

        for (int i = 0; i < currentTest.m; ++i) {
            for (int j = 0; j < currentTest.n; ++j) {
                if ((ret = mat_get(matrix_dot, i, j, &value_custom)) != SUCCESS) {
                    fprintf(stderr, "Test %d FAILED: mat_get error at (%d, %d): ", t+1, i, j);
                    print_error(ret);
                    test_passed = 0;
                    goto cleanup; 
                }
                value_blas = C[i * ldc + j];

                if (fabsf((float)value_blas - value_custom) > tolerance) {
                    fprintf(stderr, "\nTest %d FAILED: Mismatch at index (%d, %d)\n", t+1, i, j);
                    fprintf(stderr, "  Custom Library Value: %.6f\n", value_custom);
                    fprintf(stderr, "  BLAS Value:           %.6f\n", value_blas);
                    test_passed = 0;
                    goto cleanup;
                }
            }
        }

    cleanup:
        free(A);
        free(B);
        free(C);
        A = B = C = NULL;

        mat_destroy(matrix);
        mat_destroy(matrix2);
        mat_destroy(matrix_dot);
        matrix = matrix2 = matrix_dot = NULL;

        if (test_passed) {
            printf("--- Test Case %d PASSED ---\n\n", t + 1);
        } else {
            printf("--- Test Case %d FAILED ---\n\n", t + 1);
            overall_success = 0;
        }
    }

    printf("===============================\n");
    if (overall_success) {
        printf("All tests PASSED successfully!\n");
    } else {
        printf("One or more tests FAILED!\n");
    }
    printf("===============================\n");

    return overall_success ? 0 : 1;
}

int test_linear() {
    Matrix * matrix;
    Matrix * matrix2;
    Matrix * bias;
    Matrix * matrix_linear;
    RET ret;

    float A[6] = {1, 2, 3, 4, 5, 6};

    float W[6] = {4, 5, 6, 1, 2, 3};

    float b[2] = {-3, -5};

    // A = m*k, W = n*k, b = 1*n
    int m = 2;
    int n = 2;
    int k = 3;

    if ((ret = mat_create(A, m, k, &matrix)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    if ((ret = mat_create(W, n, k, &matrix2)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    if ((ret = mat_create(b, 1, n, &bias)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    mat_print(matrix);
    mat_print(matrix2);
    mat_print(bias);
    printf("\n");
    
    if ((ret = mat_linear(matrix, matrix2, bias, &matrix_linear)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    mat_print(matrix_linear);
    printf("\n");

    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(bias);
    mat_destroy(matrix_linear);
    return 0;
}

int main() {
    return test1() || test_dot() || test_linear();
}