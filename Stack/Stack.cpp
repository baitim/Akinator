#include <memory.h>
#include <stdlib.h>

#include "Stack.h"

const int DEFAULT_SIZE = 0;
const int DEFAULT_CAPACITY = 20;
const double MULTIPLIER_CAPACITY = 1.5;

enum ResizeMultiplier {
    MULTIPLIER_REDUCE   = -1,
    MULTIPLIER_INCREASE =  1
};

static StackErrors stack_resize(Stack *stack, int multiplier);

StackErrors stack_ctor(Stack *stack)
{
    stack->left_canary_struct = DEFAULT_CANARY;
    stack->right_canary_struct = DEFAULT_CANARY;
    stack->size = DEFAULT_SIZE;
    stack->capacity = DEFAULT_CAPACITY;
    type_el *new_data = (type_el *)realloc(stack->data, DEFAULT_CAPACITY * sizeof(type_el) + sizeof(long long) * 2);
    if (!new_data)
        return STACK_ERROR_ALLOC_FAIL;
    memset(new_data, POISON_BYTE, DEFAULT_CAPACITY);
    if (!new_data)
        return STACK_ERROR_ALLOC_FAIL;
    else if(stack->data != new_data)
        stack->data = new_data;

    *((long long *)stack->data) = DEFAULT_CANARY;
    stack->data = (type_el *)((long long *)stack->data + 1);
    *((long long *)stack->data + get_right_canary_index(stack)) = DEFAULT_CANARY;
    stack->hash = get_hash(stack);
    return stack_dump(stack); 
}

StackErrors stack_dtor(Stack *stack)
{
    StackErrors error = stack_dump(stack);
    if (error) return error;

    stack->data = (type_el *)((char *)stack->data - sizeof(long long));
    free(stack->data);
    stack->data = nullptr;
    stack->size = POISON_BYTE;
    stack->capacity = POISON_BYTE;
    stack->left_canary_struct = POISON_BYTE;
    stack->right_canary_struct = POISON_BYTE;
    stack->hash = POISON_BYTE;
    return error;
}

static StackErrors stack_resize(Stack *stack, int multiplier)
{
    StackErrors error = stack_dump(stack);
    if (error) return error;
    if (multiplier == MULTIPLIER_REDUCE)    stack->capacity = (int)(stack->capacity / MULTIPLIER_CAPACITY);
    if (multiplier == MULTIPLIER_INCREASE)  stack->capacity = (int)(stack->capacity * MULTIPLIER_CAPACITY);
    stack->data = (type_el *)((long long *)stack->data - 1);
    *((long long *)stack->data) = POISON_BYTE;
    *((long long *)stack->data + 1 + get_right_canary_index(stack)) = POISON_BYTE;
    stack->data = (type_el *)realloc(stack->data, stack->capacity * sizeof(type_el) + sizeof(long long) * 2);
    if (!stack->data)
        return STACK_ERROR_ALLOC_FAIL; 

    *((long long *)stack->data) = DEFAULT_CANARY;
    stack->data = (type_el *)((long long *)stack->data + 1);
    *((long long *)stack->data + get_right_canary_index(stack)) = DEFAULT_CANARY;
    stack->hash = get_hash(stack);
    return stack_dump(stack);
}

StackErrors stack_push(Stack *stack, type_el value)
{
    StackErrors error = stack_dump(stack);
    if (error) return error;

    if (stack->size >= stack->capacity)
        error = stack_resize(stack, MULTIPLIER_INCREASE);

    if (error) return error;

    stack->data[stack->size] = value;
    stack->size++;
    stack->hash = get_hash(stack);
    return stack_dump(stack);
}

StackErrors stack_pop(Stack *stack, type_el *value)
{
    StackErrors error = stack_dump(stack);
    if (error) return error;

    if (stack->size < stack->capacity / (MULTIPLIER_CAPACITY + 1))
        error = stack_resize(stack, MULTIPLIER_REDUCE);

    if (error) return error;

    stack->size--;
    *value = stack->data[stack->size];
    stack->data[stack->size] = POISON_EL;
    stack->hash = get_hash(stack);
    return stack_dump(stack);
}

int get_left_canary_index(const Stack */*stack*/)
{
    return -1;
}

int get_right_canary_index(const Stack *stack)
{
    return ((int)sizeof(type_el) * stack->capacity + ((int)sizeof(type_el) * stack->capacity) % 8) / (int)sizeof(long long);
}
