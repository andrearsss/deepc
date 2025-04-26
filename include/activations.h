#ifndef _ACTIVATIONS_H_
#define _ACTIVATIONS_H_


#define N_ACTIVATIONS 2
#define NO_ACT 0
#define RELU 1

extern const float (*ACTIVATIONS[N_ACTIVATIONS])(float x);

#endif