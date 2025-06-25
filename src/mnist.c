#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "mnist_file.h"

const char * train_images_file = "mnist/train-images.idx3-ubyte";
const char * train_labels_file = "mnist/train-labels.idx1-ubyte";
const char * test_images_file = "mnist/t10k-images.idx3-ubyte";
const char * test_labels_file = "mnist/t10k-labels.idx1-ubyte";

int main(int argc, char *argv[])
{
    mnist_dataset_t * train_dataset, * test_dataset;

    // Read the datasets from the files
    train_dataset = mnist_get_dataset(train_images_file, train_labels_file);
    test_dataset = mnist_get_dataset(test_images_file, test_labels_file);

    // Initialize network
    

    // 

    for (int i=0; i<MNIST_IMAGE_SIZE; i++) {
        printf("%-4d ", train_dataset->images[2].pixels[i]);
        if ((i + 1) % 28 == 0) printf("\n");
    }
    printf("\nLABEL: %d\n\n", train_dataset->labels[2]);

    // Cleanup
    mnist_free_dataset(train_dataset);
    mnist_free_dataset(test_dataset);

    return 0;
}