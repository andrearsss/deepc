#include "dense.h"
#include "matrix.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

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
    if (activation >= N_ACTIVATIONS)
        return INVALID_ACTIVATION;
    RET ret;

    if ((*d = malloc(sizeof(struct dense))) == NULL)
        return ALLOC_FAILED;    

    // alloc W and b
    if ((ret = mat_create(W, n_neurons, n_input, &(*d)->W)) != SUCCESS
        || (ret = mat_create(b, 1, n_neurons, &(*d)->b)) != SUCCESS)
        return ret;

    (*d)->activation = activation;
    (*d)->n_input = n_input;
    (*d)->n_neurons = n_neurons;

    return SUCCESS;
}

void dense_destroy(Dense * d) {
    free(d->W);
    free(d->b);
    free(d);
}