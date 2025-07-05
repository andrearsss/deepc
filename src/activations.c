#include <stdlib.h>
#include <activations.h>
#include <math.h>

static inline float ReLU(float x) {
    //return x * (x > 0.0f); // portable, branchless, but returns -0.0f
    //return (x > 0.0f) ? x : 0.0f; // simple, avoids -0.0f but may introduce branching
    return fmaxf(0.0f, x); // branchless, single instruction (MAXSS) on SIMD-enabled CPUs, man(3), from math.h
}

const float (*ACTIVATIONS[])(float x) = {NULL, ReLU};