
// A = m*k, B = k*n
typedef struct {
    const char* name;
    float* A;
    float* B;
    int m;
    int n;
    int k;
} TestCase;

// Test Case 1: Original 3x2 * 2x3
float A1[] = {
    1.0f, 2.0f,
    3.0f, 4.0f,
    5.0f, 6.0f
};
float B1[] = {
    7.0f,  8.0f,  9.0f,
    10.0f, 11.0f, 12.0f
};

// Test Case 2: Square Matrices 2x2 * 2x2 (Identity * Matrix)
float A2[] = {
    1.0f, 0.0f,
    0.0f, 1.0f
};
float B2[] = {
    5.0f, 6.0f,
    7.0f, 8.0f
};

// Test Case 3: Rectangle 2x3 * 3x1 (Result is 2x1 column vector)
float A3[] = {
    1.0f, 2.0f, 3.0f,
    4.0f, 5.0f, 6.0f
};
float B3[] = {
    1.0f,
    2.0f,
    3.0f
};

// Test Case 4: Zero Matrix 2x2 * 2x2
float A4[] = {
    0.0f, 0.0f,
    0.0f, 0.0f
};
float B4[] = {
    1.0f, 2.0f,
    3.0f, 4.0f
};

// Test Case 5: Rectangle 1x4 * 4x2
float A5[] = {
    1.0f, 2.0f, 3.0f, 4.0f
};
float B5[] = {
    5.0f, 6.0f,
    7.0f, 8.0f,
    9.0f, 10.0f,
    11.0f, 12.0f
};

// Test Case 6: Identity Matrix Multiplied by Identity (3x3 * 3x3)
float A6[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};
float B6[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

// Test Case 7: Random Non-Square 4x2 * 2x3
float A7[] = {
    1.0f, 2.0f,
    3.0f, 4.0f,
    5.0f, 6.0f,
    7.0f, 8.0f
};
float B7[] = {
    1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f
};

// Test Case 8: Large Square 5x5 * 5x5 (Only Diagonal Elements)
float A8[] = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 3.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 4.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 5.0f
};
float B8[] = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f
};
