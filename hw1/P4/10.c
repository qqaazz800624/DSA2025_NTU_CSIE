#include <stdio.h>
#include <stdlib.h>

#define INIT_SIZE 100000  // Initial array size

typedef struct {
    long long *data;
    int size;
    int capacity;
} DiamondCollection;

void init_collection(DiamondCollection *collection);
void insert_sorted(DiamondCollection *collection, long long vi, int Ni);
void process_type_1(DiamondCollection *collection, int Ni, long long vi);
int binary_search(long long arr[], int size, long long key);
void process_type_2(DiamondCollection *collection, long long pi);
int partition(long long arr[], int low, int high);
void quicksort(long long arr[], int low, int high);
void process_type_3(DiamondCollection *collection, long long M);

int main(){

    int T;
    long long M;
    scanf("%d %lld", &T, &M);

    DiamondCollection collection;
    init_collection(&collection);

    int i;
    for (i = 0; i < T; i++){
        int type;
        scanf("%d", &type);

        if (type == 1){
            int Ni;
            long long vi;
            scanf("%d %lld", &Ni, &vi);
            process_type_1(&collection, Ni, vi);
        } else if (type == 2){
            long long pi;
            scanf("%lld", &pi);
            process_type_2(&collection, pi);
        } else if (type == 3){
            process_type_3(&collection, M);
        }
    }
    free(collection.data);

    return 0;
}

void init_collection(DiamondCollection *collection){
    collection->capacity = INIT_SIZE;
    collection->size = 0;
    collection->data = (long long *)malloc(sizeof(long long) * collection->capacity);
    if (collection->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void insert_sorted(DiamondCollection *collection, long long vi, int Ni){
    while (collection->size + Ni > collection->capacity) {
        collection->capacity *= 2;
        collection->data = realloc(collection->data, sizeof(long long) * collection->capacity);
        if (collection->data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
    }
    
    int insert_pos = 0;
    while (insert_pos < collection->size && collection->data[insert_pos] >= vi){
        insert_pos++;
    }

    for (int i = collection->size - 1; i >= insert_pos; i--){
        collection->data[i + Ni] = collection->data[i];
    }

    for (int i = 0; i < Ni; i++){
        collection->data[insert_pos + i] = vi;
    }

    collection->size += Ni;
}

void process_type_1(DiamondCollection *collection, int Ni, long long vi){
    int i, new_size = 0, removed = 0;
    for (i = 0; i < collection->size; i++){
        if (collection->data[i] >= vi){
            collection->data[new_size++] = collection->data[i];
        } else {
            removed++;
        }
    }
    printf("%d\n", removed);
    collection->size = new_size;
    insert_sorted(collection, vi, Ni);
}

int binary_search(long long arr[], int size, long long key){
    int left = 0, right = size - 1;
    while (left <= right){
        int mid = left + (right - left) / 2;
        if (arr[mid] == key){
            return mid;
        } else if (arr[mid] < key){
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return -1;
}

void process_type_2(DiamondCollection *collection, long long pi){
    int count = 0, index = binary_search(collection->data, collection->size, pi);
    if (index != -1){
        int left = index;
        while (left >= 0 && collection->data[left] == pi){
            left--;
            count++;
        }
        int right = index + 1;
        while (right < collection->size && collection->data[right] == pi){
            count++;
            right++;
        }
    }
    printf("%d\n", count);
}

int partition(long long arr[], int low, int high){
    long long pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++){
        if (arr[j] > pivot){
            i++;
            long long temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    long long temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

void quicksort(long long arr[], int low, int high){
    if (low < high){
        int pivotIndex = partition(arr, low, high);
        quicksort(arr, low, pivotIndex - 1);
        quicksort(arr, pivotIndex + 1, high);
    }
}

void process_type_3(DiamondCollection *collection, long long M){
    quicksort(collection->data, 0, collection->size - 1);
    for (int i = 0; i < collection->size; i++){
        collection->data[i] += (M - i);
    }
}