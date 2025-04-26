#include <stdlib.h>
#include <activations.h>

static inline float ReLU(float x) {
    //return x * (x > 0.0f); // portable, branchless, but returns -0.0f
    //return fmaxf(0.0f, x); // single instruction (MAXSS) on SIMD-enabled CPUs, man(3), from math.h
    return (x > 0.0f) ? x : 0.0f;
}

const float (*ACTIVATIONS[])(float x) = {NULL, ReLU};