#include "tensor.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h> // ############

/*
[Tensor struct = int + int* + float*][shape = n_dims * int][data = n_elements * float]
*/

struct tensor {
    int n_dims;
    int * shape;    
    float * data;
};

Tensor * tensor_create(int n_dims, int * shape) {
    Tensor * tensor;
    size_t total_size;
    size_t shape_offset;
    size_t data_offset;
    int n_elements;

    if (n_dims < 1 || n_dims > 4)
        return NULL;

    // verifica shape

    n_elements = 0;
    for (int i = 0; i < n_dims; i++)
        n_elements *= shape[i];

    total_size = sizeof(struct tensor) + n_dims * sizeof(int) + n_elements * sizeof(float);
    tensor = malloc(total_size);
    if (!tensor)
        return NULL;

    tensor->n_dims = n_dims;
    shape_offset = sizeof(struct tensor) + n_dims * sizeof(int);
    tensor->shape = (int *) tensor + shape_offset;
    for (int i=0; i<n_dims; i++)
        tensor->shape[i] = shape[i];
    data_offset = shape_offset + n_dims * sizeof(int);
    //tensor->data = (float *) tensor + shape_offset;
    return tensor;
}

int tensor_get_dims(Tensor* tensor) {
    return tensor->n_dims;
}

void tensor_print_shape(Tensor* tensor) {
    printf("(");
    for (int i=0; i < tensor->n_dims; i++)
        printf("%d%s", tensor->shape[i], (i==tensor->n_dims-1) ? "" : ", ");
    printf(")");
}

void tensor_destroy(Tensor * t) {
    free(t);
}