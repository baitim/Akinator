#ifndef STACK_H
#define STACK_H

#include "../Errors.h"

typedef int type_el;
const int POISON_BYTE = -1;
const int POISON_EL = -1;

struct Stack {
    long long left_canary_struct;
    type_el *data;
    int size;
    int capacity;
    int hash;
    long long right_canary_struct;
};

struct Errors {
    int error;
    const char *description;
};

enum StackErrors {
    STACK_ERROR_NO =                          0,
    STACK_ERROR_STACK_EMPTY =                 1 << 0,
    STACK_ERROR_ALLOC_FAIL =                  1 << 1,
    STACK_ERROR_STACK_CAPACITY =              1 << 2,
    STACK_ERROR_STACK_SIZE =                  1 << 3,
    STACK_ERROR_STACK_CAPACITY_LESS_SIZE =    1 << 4,
    STACK_ERROR_LEFT_CANARY_STRUCT =          1 << 5,
    STACK_ERROR_RIGHT_CANARY_STRUCT =         1 << 6,
    STACK_ERROR_HASH =                        1 << 7,
    STACK_ERROR_LEFT_CANARY_DATA =            1 << 8,
    STACK_ERROR_RIGHT_CANARY_DATA =           1 << 9,
    STACK_ERROR_STACK_DATA_EMPTY =            1 << 10,
};

#include "Stack_dump.h"

const Errors STACK_ERRORS[] = {
    {STACK_ERROR_NO,                          "no errors"},
    {STACK_ERROR_STACK_EMPTY,                 "pointer stack is empty"},
    {STACK_ERROR_STACK_DATA_EMPTY,            "pointer stack->data is empty"},
    {STACK_ERROR_STACK_CAPACITY,              "stack->capacity < 0"},
    {STACK_ERROR_STACK_SIZE,                  "stack->size < 0"},
    {STACK_ERROR_STACK_CAPACITY_LESS_SIZE,    "stack->capacity < stack->size"},
    {STACK_ERROR_LEFT_CANARY_STRUCT,          "stack->left_canary_struct was changed"},
    {STACK_ERROR_RIGHT_CANARY_STRUCT,         "stack->right_canary_struct was changed"},
    {STACK_ERROR_HASH,                        "stack->hash was changed"},
    {STACK_ERROR_LEFT_CANARY_DATA,            "stack->left_canary_data was changed"},
    {STACK_ERROR_RIGHT_CANARY_DATA,           "stack->right_canary_data was changed"},
    {STACK_ERROR_ALLOC_FAIL,                  "realloc fail"},
};
const int COUNT_STACK_ERRORS = sizeof(STACK_ERRORS) / sizeof(Errors);

StackErrors stack_ctor(Stack *stack);

StackErrors stack_dtor(Stack *stack);

StackErrors stack_push(Stack *stack, type_el value);

StackErrors stack_pop(Stack *stack, type_el *value);

int get_left_canary_index(const Stack */*stack*/);

int get_right_canary_index(const Stack *stack);

#endif // STACK_H
