#include <stdio.h>
#include "6.h"

int main()
{
    int row;
    int column[50];
    int *array[50];
    int buffer[10000];
    scanf("%d", &row);
    for (int i = 0; i < row; i++)
    {
        scanf("%d", &column[i]);
    }
    prepare_array(buffer, array, row, column);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column[i]; j++)
        {
            scanf("%d", &array[i][j]);
        }
    }
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column[i]; j++)
        {
            printf("%d ", array[i][j]);
        }
    }
    return 0;
}