#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "dense.h"
#include "activations.h"

#define N_SAMPLES 3
#define INPUT_SIZE 5
#define N_LAYERS 5
#define N_NEURONS 15

int main() {

    RET ret;
    Dense * network[N_LAYERS];
    Matrix * input;
    Matrix * tmp;

    srand(time(NULL));

    if ((ret = mat_create(NULL, N_SAMPLES, INPUT_SIZE, HE_INIT, &input) ) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    // alloc network
    for (int i = 0; i < N_LAYERS; i++) {
        //printf("\ndense_create %d/%d", i, N_LAYERS-1);
        if ((ret = dense_create(NULL, NULL, (i==0) ? INPUT_SIZE : N_NEURONS, N_NEURONS, NO_ACT, &network[i])) != SUCCESS) {
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
    
    return 0;
}
