#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "dense.h"
#include "activations.h"

#define N_SAMPLES 2
#define INPUT_SIZE 3
#define N_LAYERS 70
#define N_NEURONS 50

// todo: check nan and inf during matrix calculations
// todo: he initialization

void fill_random(float *arr, int size, float scale) {
    for (int i = 0; i < size; i++) {
        arr[i] = ((float)rand() / RAND_MAX) * 2 * scale - scale;  // [-scale, scale]
        printf("%11.10f ", arr[i]);
    }
}

int main() {

    RET ret;
    Dense * network[N_LAYERS];
    Matrix * input;
    Matrix * tmp;

    // random input and weights
    srand(time(NULL));

    float *inp = (float *) malloc(N_SAMPLES * INPUT_SIZE * sizeof(float));
    float *W_inp = (float *) malloc(N_NEURONS * INPUT_SIZE * sizeof(float));
    float *W = (float *) malloc(N_NEURONS * N_NEURONS * sizeof(float));
    float *b = (float *) malloc(N_NEURONS * sizeof(float));
    if (!inp || !W || !b) {
        printf("Memory allocation failed\n");
        return -1;
    }

    printf("\nfill inp ");
    fill_random(inp, N_SAMPLES * INPUT_SIZE, 1.0f);
    printf("\nfill W_inp ");
    fill_random(W_inp, N_NEURONS * INPUT_SIZE, 1.0f);
    printf("\nfill W ");
    fill_random(W, N_NEURONS * N_NEURONS, 1.0f);
    printf("\nfill b ");
    fill_random(b, N_NEURONS, 1.0f);

    if ((ret = mat_create(inp, N_SAMPLES, INPUT_SIZE, &input) ) != SUCCESS) {
        print_error(ret);
        return -1;
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
            return -1;
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
