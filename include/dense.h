#ifndef _DENSE_H_
#define _DENSE_H_

#include "error.h"

typedef struct dense Dense;

#define N_ACTIVATIONS 1
#define RELU 0

RET dense_create(const float * W, const float * b, int n_input, int n_neurons, int activation, Dense ** d);
void dense_destroy(Dense * d);


#endif