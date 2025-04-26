#include <stdio.h>
#include "matrix.h"
#include "dense.h"
#include "activations.h"

#define N_SAMPLES 2
#define INPUT_SIZE 3
#define N_LAYERS 100
#define N_NEURONS 5

int main() {

    RET ret;
    Dense * network[N_LAYERS];
    Matrix * input;
    Matrix * tmp;

    const float inp[N_SAMPLES*INPUT_SIZE] = {1, 2, 3,
                         4, 5, 6}; 
    const float W[N_NEURONS*INPUT_SIZE] = {4, -5, 6,
                                        7, 8, 9,
                                        10, -11, 12,
                                        1, -2 , 3,
                                        13, -14, 15};
    const float b[N_NEURONS] = {-16, 2, -3, 1, 0};

    if ((ret = mat_create(inp, N_SAMPLES, INPUT_SIZE, &input) ) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    for (int i = 0; i < N_LAYERS; i++) {
        printf("\ndense_create %d/%d", i, N_LAYERS-1);
        if ((ret = dense_create(W, b, (i==0) ? INPUT_SIZE : N_NEURONS, N_NEURONS, RELU, &network[i])) != SUCCESS) {
            print_error(ret);
            return 1;
        }
    }
    
    for (int i = 0; i < N_LAYERS; i++) {
        printf("\ndense_forward %d/%d", i, N_LAYERS-1);
        if ((ret = dense_forward(network[i], input, &tmp)) != SUCCESS) {
            print_error(ret);
            return 1;
        }
        mat_destroy(input);
        input = tmp;
    }
    
    printf("\nOutput:");
    mat_print(tmp);

    mat_destroy(tmp);
    for (int i = 0; i < N_LAYERS; i++)
        dense_destroy(network[i]);

    return 0;
}
