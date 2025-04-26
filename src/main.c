#include <stdio.h>
#include "matrix.h"
#include "dense.h"
#include "activations.h"
#include "cblas.h"

int main() {

    RET ret;
    Dense * d;
    Matrix * input;
    Matrix * output;

    const float inp[6] = {1, 2, 3,          // 2 samples of size 3
                         4, 5, 6}; 
    const float W[9] = {1, 2 , 3,           // 3 rows = 3 neurons
                        4, 5, 6,
                        7, 8, 9};
    const float b[3] = {1, 2, 3};           // 1 per neuron

    if ((ret = dense_create(W, b, 3, 3, NO_ACT, &d)) != SUCCESS)
        return ret;

    if ((ret = mat_create(inp, 2, 3, &input) ) != SUCCESS)
        return ret;

    if ((ret = dense_forward(input, d, &output)) != SUCCESS)
        return ret;
        
    mat_print(output);

    return 0;
}

