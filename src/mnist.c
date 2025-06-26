#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "mnist_file.h"
#include "dense.h"
#include "activations.h"

#define N_SAMPLES 3
#define INPUT_SIZE MNIST_IMAGE_SIZE
#define N_LAYERS 4
#define N_NEURONS 64
#define N_CLASSES 10

const char * train_images_file = "mnist/train-images.idx3-ubyte";
const char * train_labels_file = "mnist/train-labels.idx1-ubyte";
const char * test_images_file = "mnist/t10k-images.idx3-ubyte";
const char * test_labels_file = "mnist/t10k-labels.idx1-ubyte";

int main(int argc, char *argv[])
{
    RET ret;
    mnist_dataset_t * train_dataset, * test_dataset;
    Dense * network[N_LAYERS];
    float * weights;
    int w_offset, b_offset;

    // Read the datasets from the files
    train_dataset = mnist_get_dataset(train_images_file, train_labels_file);
    test_dataset = mnist_get_dataset(test_images_file, test_labels_file);


    // load weights
    size_t total_floats = (INPUT_SIZE*N_NEURONS + N_NEURONS)              // first layer
                        + (N_LAYERS-2)*(N_NEURONS*N_NEURONS + N_NEURONS)    // hidden layers
                        + (N_NEURONS*N_CLASSES + N_CLASSES);                  //output layer
    weights = load_weights("mnist/weights.bin", total_floats);


    // alloc network
    // first layer
    b_offset = INPUT_SIZE*N_NEURONS;
    if ((ret = dense_create(weights, weights+b_offset, INPUT_SIZE, N_NEURONS, RELU, &network[0])) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    // hidden layers
    w_offset = INPUT_SIZE*N_NEURONS + N_NEURONS;
    b_offset = w_offset + N_NEURONS*N_NEURONS;
    for (int i = 1; i < N_LAYERS-1; i++) {
        if (i != 1) {
            w_offset += N_NEURONS*N_NEURONS + N_NEURONS;
            b_offset += N_NEURONS*N_NEURONS + N_NEURONS;
        }
        if ((ret = dense_create(weights+w_offset, weights+b_offset, N_NEURONS, N_NEURONS, RELU, &network[i])) != SUCCESS) {
            print_error(ret);
            return 1;
        }
    }

    // last layer
    w_offset += N_NEURONS*N_NEURONS + N_NEURONS;
    b_offset += N_NEURONS*N_CLASSES + N_NEURONS;
    if ((ret = dense_create(weights+w_offset, weights+b_offset, N_NEURONS, N_CLASSES, NO_ACT, &network[N_LAYERS-1])) != SUCCESS) {
        print_error(ret);
        return 1;
    }

    // test img
    printf("\n");
    for (int i=0; i<MNIST_IMAGE_SIZE; i++) {
        printf("%-4d ", test_dataset->images[0].pixels[i]);
        if ((i + 1) % 28 == 0) printf("\n");
    }
    printf("\nLABEL: %d\n\n", test_dataset->labels[0]);

    // convert input to float
    Matrix * input;
    Matrix * tmp;
    float pixels_float[MNIST_IMAGE_SIZE];

    for (int i = 0; i < MNIST_IMAGE_SIZE; ++i) {
        pixels_float[i] = (float)(test_dataset->images[0].pixels[i]);
    }

    if ( (ret = mat_create(pixels_float, 1, MNIST_IMAGE_SIZE, NO_INIT, &input)) != SUCCESS) {
        print_error(ret);
        return 1;
    }
    //mat_print(input);

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


    // Cleanup
    mnist_free_dataset(train_dataset);
    mnist_free_dataset(test_dataset);
    free(weights);


    return 0;
}