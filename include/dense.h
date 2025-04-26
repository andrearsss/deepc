#ifndef _DENSE_H_
#define _DENSE_H_

#include "error.h"
#include "matrix.h"

typedef struct dense Dense;

RET dense_create(const float * W, const float * b, int n_input, int n_neurons, int activation, Dense ** d);
void dense_destroy(Dense * d);

RET dense_forward(Dense * d, const Matrix * input, Matrix ** out);


#endif