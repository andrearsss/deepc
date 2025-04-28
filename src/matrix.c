#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

#define M_PI 3.14159265358979323846

#define MAX_ROWS 1000
#define MAX_COLS 1000

#define OP_ADD_EW 0
#define OP_MUL_EW 1

struct matrix {
    int n_rows;
    int n_cols;    
    float data[];
};

static inline float _random_normal();
static RET _mat_init(Matrix *m, int init);
static RET _mat_ew_op(Matrix * m1, const Matrix * m2, int op);

RET mat_create(const float * data, int nr, int nc, int init, Matrix ** m) {
    /*
        Note: init is ignored if a valid data pointer is provided
    */
    size_t total_size;

    if (nr < 1 || nr > MAX_ROWS || nc < 1 || nc > MAX_COLS)
        return INVALID_DIM;

    total_size = sizeof(struct matrix) + nr * nc * sizeof(float);

    if ((*m = malloc(total_size)) == NULL)
        return ALLOC_FAILED;

    (*m)->n_rows = nr;
    (*m)->n_cols = nc;

    if (data != NULL) { 
        for (int i = 0; i < nr*nc; i++)
            (*m)->data[i] = data[i];
    }
    else if (init != NO_INIT) {
        return _mat_init(*m, init);
    }
    return SUCCESS;
}

inline void mat_destroy(Matrix * m) {
    free(m);
}

RET mat_copy(const Matrix * m, Matrix ** m_copy) {
    if (m == NULL)
        return NULL_POINTER;

    return mat_create(m->data, m->n_rows, m->n_cols, NO_INIT, m_copy);
}

RET mat_get(const Matrix * m, int i, int j, float * out) {
    if (m == NULL || out == NULL)
        return NULL_POINTER;

    if (i < 0 || i >= m->n_rows || j < 0 || j >= m->n_cols)
        return INVALID_DIM;

    *out = m->data[i * m->n_cols + j];
    return SUCCESS;
}

RET mat_transpose(Matrix * m) {
    if (m == NULL)
        return NULL_POINTER;

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
    return SUCCESS;
}

RET mat_add_bias(Matrix * m, int bias) {
    if (m == NULL)
        return NULL_POINTER;
    if (bias == 0)
        return SUCCESS;
    float tmp;
    for (int i = 0; i < m->n_rows*m->n_cols; i++) {
        m->data[i] += bias;
        tmp = m->data[i];
        if (isnan(tmp) || isinf(tmp))
            return NUMERICAL_ERROR;
    }
    return SUCCESS;
}

RET mat_add_ew(Matrix * m1, const Matrix * m2) {
    return _mat_ew_op(m1, m2, OP_ADD_EW);
}

RET mat_mul_ew(Matrix * m1, const Matrix * m2) {
    return _mat_ew_op(m1, m2, OP_MUL_EW);
}

RET mat_dot(Matrix * m1, const Matrix * m2, Matrix ** m) {
    if (m1 == NULL || m2 == NULL)
        return NULL_POINTER;
    if (m1->n_cols != m2->n_rows)
        return INVALID_SHAPE;
        
    float sum;
    int ret, i, j, row, col;
    int m1_nr = m1->n_rows;
    int m1_nc = m1->n_cols;
    int m2_nc = m2->n_cols;
    Matrix * m2_T;

    // copy m2 and transpose -> m2_T
    // O(N*M)
    if (((ret = mat_copy(m2, &m2_T)) != SUCCESS)
        || ((ret = mat_transpose(m2_T)) != SUCCESS)) {
        return ret;
    }
    
    // alloc m = nr1*nc2
    if ((ret = mat_create(NULL, m1_nr, m2_nc, NO_INIT, m)) != SUCCESS)
        return ret;

    // row per row product
    // O(N1*M2*M1)
    for (i = 0; i < m1_nr*m2_nc; i++) {
        row = i / m2_nc;   // row iterator for m1
        col = i % m2_nc;   // col iterator for m2
        sum = 0;
        for (j = 0; j < m1_nc; j++) {
            sum += m1->data[row*m1_nc + j] * m2_T->data[col*m1_nc + j];
        }
        if (isnan(sum) || isinf(sum))
            return NUMERICAL_ERROR;
        (*m)->data[i] = sum;
    }

    free(m2_T);
    return SUCCESS;
}

RET mat_linear_activation(const Matrix * m1, const Matrix * m2_T, const Matrix * bias, float (*act)(float), Matrix ** m_out) {
    // Performs row-wise dot product while adding bias,
    // then applies an optional activation function (act)
    if (!m1 || !m2_T || !bias || !m_out)
        return NULL_POINTER;
    if (m1->n_cols != m2_T->n_cols || bias->n_cols != m2_T->n_rows || bias->n_rows != 1)
        return INVALID_SHAPE;

    RET ret;
    float sum, b;
    int i, j, row, col;
    int m1_nr = m1->n_rows;
    int m2_nr = m2_T->n_rows;
    int m1_nc = m1->n_cols;

    if ((ret = mat_create(NULL, m1_nr, m2_nr, NO_INIT, m_out)) != SUCCESS)
        return ret;

    // row per row product
    // O(N1*M2*M1)
    for (i = 0; i < m1_nr*m2_nr; i++) {
        row = i / m2_nr;   // row iterator for m1
        col = i % m2_nr;   // col iterator for m2
        if ((ret = mat_get(bias, 0, col, &b)) != SUCCESS)
            return ret;
        sum = b;
        for (j = 0; j < m1_nc; j++) {
            sum += m1->data[row*m1_nc + j] * m2_T->data[col*m1_nc + j];
        }
        if (isnan(sum) || isinf(sum))
            return NUMERICAL_ERROR;
        (*m_out)->data[i] = (act == NULL) ? sum : act(sum);     // optional activation function

        // todo: test this, should be faster than divisions and modulus
        /* col++;
        if (col == m2_nr) {
            col = 0;
            row++;
        } */
    }
    return SUCCESS;
}

void mat_print(const Matrix * m) {
    if (m == NULL)
        return;
    printf("\n");
    for (int i = 0; i < m->n_rows*m->n_cols; i++) {
        printf("%11.10f%s", m->data[i], (((i+1) % (m->n_cols)) == 0) ? "\n" : " ");
    }
}

// private

RET _mat_ew_op(Matrix * m1, const Matrix * m2, int op) {
    if (m1 == NULL || m2 == NULL)
        return NULL_POINTER;
    if ((m1->n_rows != m2->n_rows) || (m1->n_cols != m2->n_cols))
        return INVALID_SHAPE;

    float tmp;
    int nr = m1->n_rows;
    int nc = m1->n_cols;

    for (int i = 0; i < nr*nc; i++) {
        if (op == OP_ADD_EW){
            m1->data[i] += m2->data[i];
        } else if (op == OP_MUL_EW) {
            m1->data[i] *= m2->data[i];
        }
        tmp = m1->data[i];
        if (isnan(tmp) || isinf(tmp))
            return NUMERICAL_ERROR;
    }
    return SUCCESS;
}

static inline float _random_normal() {
    double u1 = (rand() + 1.0) / (RAND_MAX + 2.0);
    double u2 = (rand() + 1.0) / (RAND_MAX + 2.0);
    return (float) (sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2));
}

static RET _mat_init(Matrix *m, int init) {
    if (m == NULL)
        return NULL_POINTER;
    if (m->n_rows < 1 || m->n_cols < 1)
        return INVALID_SHAPE;

    int n_out = m->n_rows;  // assuming m is transposed
    int n_in = m->n_cols;

    switch (init) {
        case HE_INIT:
            float stddev = sqrt(2.0 / n_in);
            for (int i = 0; i < n_in * n_out; i++)
                m->data[i] = _random_normal() * stddev;
            break;
        case ZERO_INIT:
            for (int i = 0; i < n_in * n_out; i++)
                m->data[i] = 0;
            break;
    }
    return SUCCESS;
}