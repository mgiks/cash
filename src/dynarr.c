#include "../include/dynarr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DynArr new_dynarr(size_t elem_size) {
    DynArr arr = {._data = malloc(elem_size),
                  ._size = 0,
                  ._capacity = 1,
                  ._elem_size = elem_size};

    if (!arr._data) {
        fprintf(stderr, "Unable to allocate dynamic array\n");
        exit(1);
    }

    return arr;
}

void push_to_dynarr(DynArr *arr, void *elem) {
    if (arr->_size == arr->_capacity) {
        size_t new_capacity = arr->_capacity * 2;

        void *reallocated_data = malloc(new_capacity * arr->_elem_size);
        if (!reallocated_data) {
            fprintf(stderr,
                    "Unable to allocate more space for dynamic array\n");
            exit(1);
        }

        memcpy(reallocated_data, arr->_data, arr->_size * arr->_elem_size);
        free(arr->_data);

        arr->_data = reallocated_data;
        arr->_capacity = new_capacity;
    }

    memcpy((char *)arr->_data + arr->_size * arr->_elem_size, elem,
           arr->_elem_size);
    ++arr->_size;
}

void pop_from_dynarr(DynArr *arr) {
    if (arr->_size == 0) {
        fprintf(stderr, "Unable to pop from dynamic array\n");
        exit(1);
    }
    --arr->_size;
}

void get_from_dynarr(void *out, DynArr *arr, size_t i) {
    if (i >= arr->_size) {
        fprintf(stderr, "Unable to get from dynamic array\n");
        exit(1);
    }

    memcpy(out, (char *)arr->_data + i * arr->_elem_size, arr->_elem_size);
}

void free_dynarr(DynArr *arr) {
    free(arr->_data);
    arr->_data = NULL;
    arr->_size = arr->_capacity = 0;
}
