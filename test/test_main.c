#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cblas.h>

#include "matrix.h"
#include "dense.h"
#include "activations.h"
#include "tests.h"

#define N_SAMPLES 2
#define INPUT_SIZE 3
#define N_LAYERS 5
#define N_NEURONS 20

void fill_random(float *arr, int size, float scale) {
    for (int i = 0; i < size; i++) {
        arr[i] = ((float)rand() / RAND_MAX) * 2 * scale - scale;  // uniform in [-scale, scale]
    }
}

int test1() {
    Matrix * matrix;
    RET ret;
    const float data[] = { 1, 2, 3.110454,
                     4, 5, 6,
                     7, 8, 9};
    
    if ((ret = mat_create(data, 3, 3, NO_INIT, &matrix)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    
    //mat_print(matrix);
    //printf("+5");
    //mat_add_bias(matrix, 5);
    //mat_print(matrix);

    Matrix * matrix2;
    if ((ret = mat_create(data, 3, 3, NO_INIT, &matrix2)) != SUCCESS) {
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

    printf("Starting matrix dot test\n");
    printf("===============================\n\n");

    for (int t = 0; t < num_tests; ++t) {
        TestCase currentTest = tests[t];
        printf("--- Test Case %d: %s ---\n", t + 1, currentTest.name);
        printf("Dimensions: A(%d x %d), B(%d x %d)\n",
               currentTest.m, currentTest.k, currentTest.k, currentTest.n);
        
        int test_passed = 1;

        if ((ret = mat_create(currentTest.A, currentTest.m, currentTest.k, NO_INIT, &matrix)) != SUCCESS) {
            fprintf(stderr, "Test %d FAILED: mat_create (A) error: ", t + 1);
            print_error(ret);
            test_passed = 0;
            goto cleanup;
        }
        if ((ret = mat_create(currentTest.B, currentTest.k, currentTest.n, NO_INIT, &matrix2)) != SUCCESS) {
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

        // BLAS dgemm
        // C = alpha*A*B + beta*C
        // C = 1.0 * A * B + 0.0 * C
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

        // Check correctness comparing results with tolerance
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

                if (fabsf(value_blas - (double) value_custom) > tolerance) {
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

    printf("\nStarting matrix linear combination test\n==================================");
    if ((ret = mat_create(A, m, k, NO_INIT, &matrix)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    if ((ret = mat_create(W, n, k, NO_INIT, &matrix2)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    if ((ret = mat_create(b, 1, n, NO_INIT, &bias)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    printf("\nInput:");
    mat_print(matrix);
    printf("\nW:");
    mat_print(matrix2);
    printf("\nb:");
    mat_print(bias);
    printf("\n");
    
    if ((ret = mat_linear_activation(matrix, matrix2, bias, NO_ACT, &matrix_linear)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    printf("\nResult::");
    mat_print(matrix_linear);
    printf("\n");

    mat_destroy(matrix);
    mat_destroy(matrix2);
    mat_destroy(bias);
    mat_destroy(matrix_linear);
    return 0;
}

int test_dense() {

    RET ret;
    Dense * d;
    Matrix * input;
    Matrix * output;

    printf("Starting dense forward test\n");
    printf("===============================\n\n");

    const float inp[6] = {1, 2, 3,          // 2 samples of size 3
                         4, 5, 6}; 
    const float W[9] = {1, 2 , 3,           // 3 rows = 3 neurons
                        4, 5, 6,
                        7, 8, 9};
    const float b[3] = {-16, 2, 3};           // 1 per neuron

    if ((ret = dense_create(W, b, 3, 3, NO_ACT, &d)) != SUCCESS)
        return ret;

    if ((ret = mat_create(inp, 2, 3, NO_INIT, &input) ) != SUCCESS)
        return ret;

    if ((ret = dense_forward(d, input, &output)) != SUCCESS)
        return ret;
        
    mat_print(output);

    // check using BLAS

    double input_d[6];
    double W_d[9];
    double b_d[3];

    for (int i = 0; i < 6; ++i) input_d[i] = (double) inp[i];
    for (int i = 0; i < 9; ++i) W_d[i] = (double) W[i];
    for (int i = 0; i < 3; ++i) b_d[i] = (double) b[i];

    // output matrix
    double output_d[6] = {0}; // 2 samples x 3 neurons

    // output = input * W using cblas_dgemm
    // cblas_dgemm expects column-major by default
    cblas_dgemm(
        CblasRowMajor,    // storage order
        CblasNoTrans,     // input is not transposed
        CblasTrans,       // W is transposed
        2,                // number of rows of input (M)
        3,                // number of columns of W (N)
        3,                // shared dimension (K)
        1.0,              // alpha
        input_d,          // A
        3,                // lda (leading dimension of A)
        W_d,              // B
        3,                // ldb (leading dimension of B)
        0.0,              // beta
        output_d,         // C
        3                 // ldc (leading dimension of C)
    );

    // add bias manually
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            output_d[i * 3 + j] += b_d[j];
        }
    }
    printf("\nBLAS output:\n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%8.4f ", output_d[i * 3 + j]);
        }
        printf("\n");
    }

    // check elem by elem
    int success = 1;
    float value_custom;
    const double tolerance = 1e-6;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            if ((ret = mat_get(output, i, j, &value_custom)) != SUCCESS){
                print_error(ret);
                return 1;
            }
            double value_blas = output_d[i * 3 + j];
            if (fabs((double) value_custom - value_blas) > tolerance) {
                printf("Mismatch at (%d, %d): lib = %f, blas = %f\n", i, j, value_custom, value_blas);
                success = 0;
            }
        }
    }

    mat_destroy(input);
    mat_destroy(output);
    dense_destroy(d);

    if (success) {
        printf("\nTest PASSED\n===============================\n");
        return 0;
    } else {
        printf("\nTest FAILED: outputs differ\n==========================\n");
        return 1;
    }
}

int test_network() {

    RET ret;
    Dense * network[N_LAYERS];
    Matrix * input;
    Matrix * tmp;

    printf("Starting dense forward test\n");
    printf("===============================\n\n");
    // random input and weights
    srand(time(NULL));

    float *inp = (float *) malloc(N_SAMPLES * INPUT_SIZE * sizeof(float));
    float *W_inp = (float *) malloc(N_NEURONS * INPUT_SIZE * sizeof(float));
    float *W = (float *) malloc(N_NEURONS * N_NEURONS * sizeof(float));
    float *b = (float *) malloc(N_NEURONS * sizeof(float));
    if (!inp || !W || !b) {
        printf("Memory allocation failed\n");
        return 1;
    }

    fill_random(inp, N_SAMPLES * INPUT_SIZE, 1.0f);
    fill_random(W_inp, N_NEURONS * INPUT_SIZE, 1.0f);
    fill_random(W, N_NEURONS * N_NEURONS, 1.0f);
    fill_random(b, N_NEURONS, 1.0f);

    if ((ret = mat_create(inp, N_SAMPLES, INPUT_SIZE, NO_INIT, &input) ) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    // alloc network
    for (int i = 0; i < N_LAYERS; i++) {
        //printf("\ndense_create %d/%d", i, N_LAYERS-1);
        if ((ret = dense_create((i==0) ? W_inp : W, b, (i==0) ? INPUT_SIZE : N_NEURONS, N_NEURONS, NO_ACT, &network[i])) != SUCCESS) {
            print_error(ret);
            return -1;
        }
    }
    
    // forward
    for (int i = 0; i < N_LAYERS; i++) {
        //printf("\ndense_forward %d/%d", i, N_LAYERS-1);
        if ((ret = dense_forward(network[i], input, &tmp)) != SUCCESS) {
            print_error(ret);
            return 1;
        }
        mat_destroy(input);
        input = tmp;
    }
    
    printf("\nOutput:");
    mat_print(tmp);

    // free mem
    mat_destroy(tmp);
    for (int i = 0; i < N_LAYERS; i++)
        dense_destroy(network[i]);
    
    free(inp);
    free(W_inp);
    free(W);
    free(b);
    return 0;
}


int main() {
    return test1() || test_dot() || test_linear() || test_dense() || test_network();
}