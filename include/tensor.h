#ifndef _TENSOR_H_
#define _TENSOR_H_

typedef struct tensor Tensor;


Tensor * tensor_create(int, int *);
int tensor_get_dims(Tensor *);
void tensor_destroy(Tensor *);
void tensor_print_shape(Tensor *);


#endif