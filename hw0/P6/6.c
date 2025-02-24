#include "6.h"

void prepare_array(int buffer[], int *array[], int row, int column[]) {
    int offset = 0;
    int i;
    for (i = 0; i < row; i++){
        array[i] = &buffer[offset];
        offset += column[i];
    }
}