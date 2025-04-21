#include "matrix.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h> // ############

#define MAX_ROWS 1000
#define MAX_COLS 1000

#define OP_ADD_EW 0
#define OP_MUL_EW 1

struct matrix {
    int n_rows;
    int n_cols;    
    float * data;
};

static MAT_RET mat_ew_op(const Matrix * m1, const Matrix * m2, int op);

MAT_RET mat_create(const float * data, int nr, int nc, Matrix ** m) {
    /*
        Memory allocation:
        [struct = int + int + float*]
        [data = n_elements * float]
    */
    size_t total_size;

    if (nr < 1 || nc > MAX_ROWS || nc < 1 || nc > MAX_COLS)
        return MAT_INVALID_DIM;

    total_size = sizeof(struct matrix) + nr * nc * sizeof(float);

    if ((*m = malloc(total_size)) == NULL)
        return MAT_ALLOC_FAILED;

    (*m)->n_rows = nr;
    (*m)->n_cols = nc;
    (*m)->data = (float *) (*m + 1);     // add struct offset

    if (data != NULL) { 
        for (int i = 0; i < nr*nc; i++) { // macro?
            (*m)->data[i] = data[i];
        }
    }
    return MAT_SUCCESS;
}

MAT_RET mat_add_bias(const Matrix * m, int bias) {
    if (m == NULL)
        return MAT_NULL_POINTER;
    if (bias == 0)
        return MAT_SUCCESS;
    for (int i = 0; i < m->n_rows*m->n_cols; i++) { // macro?
        m->data[i] += bias;
    }
    return MAT_SUCCESS;
}

MAT_RET mat_add_ew(const Matrix * m1, const Matrix * m2) {
    return mat_ew_op(m1, m2, OP_ADD_EW);
}

MAT_RET mat_mul_ew(const Matrix * m1, const Matrix * m2) {
    return mat_ew_op(m1, m2, OP_MUL_EW);
}

//Matrix * mat_dot(Matrix * m1, Matrix * m2)

void mat_print(const Matrix * m) {
    if (m == NULL)
        return;
    printf("\n");
    for (int i = 0; i < m->n_rows*m->n_cols; i++) { // macro?
        printf("%8.2f%s", m->data[i], (((i+1) % (m->n_cols)) == 0) ? "\n" : " ");
    }
}

void mat_destroy(Matrix * m) {
    free(m);
}

// private

MAT_RET mat_ew_op(const Matrix * m1, const Matrix * m2, int op) {
    if (m1 == NULL || m2 == NULL)
        return MAT_NULL_POINTER;
    if ((m1->n_rows != m2->n_rows) || (m1->n_cols != m2->n_cols))
        return MAT_INVALID_SHAPE;

    int nr = m1->n_rows;
    int nc = m1->n_cols;

    for (int i = 0; i < nr*nc; i++) { // macro ?
        if (op == OP_ADD_EW)
            m1->data[i] += m2->data[i];
        else if (op == OP_MUL_EW)
            m1->data[i] *= m2->data[i];
    }
    return MAT_SUCCESS;
}

const char * mat_error_string(MAT_RET err) {
    switch (err) {
        case MAT_SUCCESS: return "Success";
        case MAT_NULL_POINTER: return "Null pointer";
        case MAT_ALLOC_FAILED: return "Allocation failed";        
        case MAT_INVALID_DIM: return "Invalid dimension(s)";
        case MAT_INVALID_SHAPE: return "Invalid tensor shape";
        default: return "Unknown error";
    }
}