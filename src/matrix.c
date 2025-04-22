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

MAT_RET mat_copy(const Matrix * m, Matrix ** m_copy) {
    if (m == NULL)
        return MAT_NULL_POINTER;

    return mat_create(m->data, m->n_rows, m->n_cols, m_copy);
}

MAT_RET mat_transpose(Matrix * m) {
    if (m == NULL)
        return MAT_NULL_POINTER;

    int i, j;
    int nr = m->n_rows;
    int nc = m->n_cols;
    
    float * t_data = malloc(nr * nc * sizeof(float)); // tmp vector to store serialized transposed values

    for (i = 0; i < nc; i++) {
        for (j = 0; j < nr; j++) {
            t_data[i*nr + j] = m->data[j*nc + i];
        }   
    }

    // in-place operation
    for (i = 0; i < nr*nc; i++)
        m->data[i] = t_data[i];
    
    // swap dims
    nc = m->n_rows;
    m->n_rows = m->n_cols;
    m->n_cols = nc;
    
    free(t_data);
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

MAT_RET mat_dot(const Matrix * m1, const Matrix * m2, Matrix ** m) {
    if (m1 == NULL || m2 == NULL)
        return MAT_NULL_POINTER;
    if (m1->n_cols != m2->n_rows)
        return MAT_INVALID_SHAPE;

    int ret, i, j, sum, row, col;
    Matrix * m2_copy;

    // copy m2 -> m2_copy
    if ((ret = mat_copy(m2, &m2_copy)) != MAT_SUCCESS) {
        return ret;
    }

    // transpose m2_copy in-place
    if ((ret = mat_transpose(m2_copy)) != MAT_SUCCESS) {
        return ret;
    }

    // m1 = nr1*nc1
    // m2 = nr2*nc2
    // alloc m = nr1*nc2
    if ((ret = mat_create(NULL, m1->n_rows, m2->n_cols, m)) != MAT_SUCCESS) {
        mat_print_error(ret);
        return 0;
    }

    // row per row product
    for (i = 0; i < m1->n_rows*m2->n_cols; i++) {
        sum = 0;
        row = i / m2->n_cols;
        col = i % m2->n_cols;
        for (j = 0; j < m2_copy->n_cols; j++)
            sum += m1->data[row*m1->n_cols + j] * m2_copy->data[col*m2_copy->n_cols + j];
        (*m)->data[i] = sum;
    }

    return MAT_SUCCESS;
}

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
        case MAT_INVALID_SHAPE: return "Invalid shape";
        default: return "Unknown error";
    }
}