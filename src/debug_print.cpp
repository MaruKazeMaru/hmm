#include <stdio.h>

#include <debug_print.hpp>

void print_matrix(int row_size, int col_size, float** mat){
    for(int i = 0; i < row_size; ++i){
        printf("%f", mat[i][0]);
        for(int j = 1; j < col_size; ++j)
            printf(" , %f", mat[i][j]);
        printf("\n");
    }
    return;
}

void print_vector(int size, float* vec){
    printf("%f", vec[0]);
    for(int i = 1; i < size; ++i)
        printf(" , %f", vec[i]);
    return;
}