#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "dense.h"
#include "matrix.h"
#include "activations.h"

#define MAX_INPUT 1000
#define MAX_NEURONS 1000

struct dense {
    int activation;
    int n_input;            // cols of W
    int n_neurons;          // rows of W
    //int batch_size;         // n. of input samples
    Matrix * W;             // n_neurons * n_input
    Matrix * b;             // 1 * n_neurons
    Matrix * pre_act;
    Matrix * deltas;        // n_neurons * n_sample (gradient flow coming from next layer * derivative of activation func)
};

RET dense_create(const float * W, const float * b, int n_input, int n_neurons, int activation, Dense ** d) {
    if (n_input < 1 || n_input > MAX_INPUT || n_neurons < 1 || n_neurons > MAX_NEURONS)
        return INVALID_DIM;
    if (activation < 0 || activation > N_ACTIVATIONS)
        return INVALID_ACTIVATION;
    RET ret;

    if ((*d = malloc(sizeof(struct dense))) == NULL)
        return ALLOC_FAILED;
    
    if ((ret = mat_create(W, n_neurons, n_input, (W == NULL) ? HE_INIT : NO_INIT, &(*d)->W)) != SUCCESS  // He initialization if W is null
        || (ret = mat_create(b, 1, n_neurons, (b == NULL) ? ZERO_INIT : NO_INIT, &(*d)->b)) != SUCCESS)    // 0 init if b is null
        return ret;

    (*d)->activation = activation;
    (*d)->n_input = n_input;
    (*d)->n_neurons = n_neurons;

    return SUCCESS;
}

RET dense_forward(Dense * d, const Matrix * input, Matrix ** out) {
    RET ret;
    if ((ret = mat_linear(input, d->W, d->b, out)) != SUCCESS
        || (ret = mat_copy(*out, &d->pre_act)) != SUCCESS)      // store pre-activations for backward pass
        return ret;
    if (d->activation && (ret = mat_apply(*out, ACTIVATIONS[d->activation])) != SUCCESS)    // apply activation if not null
        return ret;
    return SUCCESS;
}

void dense_destroy(Dense * d) {
    mat_destroy(d->W);
    mat_destroy(d->b);
    mat_destroy(d->pre_act);
    free(d);
}