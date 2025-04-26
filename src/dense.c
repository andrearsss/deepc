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
    Matrix * W;             // dims: n_neurons * n_input
    Matrix * b;             // dims: 1 * n_neurons
};

RET dense_create(const float * W, const float * b, int n_input, int n_neurons, int activation, Dense ** d) {
    if (n_input < 1 || n_input > MAX_INPUT || n_neurons < 1 || n_neurons > MAX_NEURONS)
        return INVALID_DIM;
    if (activation < 0 || activation > N_ACTIVATIONS)
        return INVALID_ACTIVATION;
    RET ret;

    if ((*d = malloc(sizeof(struct dense))) == NULL)
        return ALLOC_FAILED;    
    
    // todo: init if W and b null
    //if (W == NULL) {

    //}

    if ((ret = mat_create(W, n_neurons, n_input, &(*d)->W)) != SUCCESS
        || (ret = mat_create(b, 1, n_neurons, &(*d)->b)) != SUCCESS)
        return ret;

    (*d)->activation = activation;
    (*d)->n_input = n_input;
    (*d)->n_neurons = n_neurons;

    return SUCCESS;
}

RET dense_forward(Dense * d, const Matrix * input, Matrix ** out) {
    return mat_linear_activation(input, d->W, d->b, ACTIVATIONS[d->activation], out);
}

void dense_destroy(Dense * d) {
    mat_destroy(d->W);
    mat_destroy(d->b);
    free(d);
}