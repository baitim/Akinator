#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../ANSI_colors.h"
#include "Stack_dump.h"

static int stack_check_error(Stack *stack);
static void print_error(int error, const char *s);
static int powf(int x, int st);
static int calculate_hash(void *data, int size);
static void print_stack(Stack *stack);
static void print_el(const type_el* elem);

StackErrors stack_dump_(Stack *stack, const char *file, const char *func, int line, const char *stk)
{
    StackErrors type_error = (StackErrors)stack_check_error(stack);

    if (type_error == STACK_ERROR_NO) {
        //fprintf(stderr, print_lgreen("stack_dump: OK\n"));
        return STACK_ERROR_NO;
    }

    fprintf(stderr, print_lred("ERROR: in stack[%p], called from FILE = %s, FUNCTION = %s, LINE = %d, with %s\n"), 
                               stack, file, func, line, stk);

    print_stack_pointers(stack);

    for (int i = 1; i < COUNT_STACK_ERRORS; i++) {
        if (type_error & powf(2, i - 1))
            print_error(type_error, STACK_ERRORS[i].description);
    }
    print_stack(stack);
    return type_error;
}

static void print_stack(Stack *stack)
{
    if (stack && stack->data) {
        for (int i = 0; i < stack->size; i++) {
            printf(print_lcyan("i = %d "), i);
            print_el(&stack->data[i]);
        }
    }
}

static void print_el(const type_el* elem)
{
    printf(print_lcyan("x = %d\n"), *elem);
}

static int stack_check_error(Stack *stack)
{
    int error = STACK_ERROR_NO;
    if (!stack)                                        return error |= STACK_ERROR_STACK_EMPTY;
    if (!stack->data)                                  return error |= STACK_ERROR_STACK_DATA_EMPTY;
    if (stack->capacity < 0)                           return error |= STACK_ERROR_STACK_CAPACITY;
    if (stack->size < 0)                               return error |= STACK_ERROR_STACK_SIZE;
    if (stack->capacity < stack->size)                 return error |= STACK_ERROR_STACK_CAPACITY_LESS_SIZE;
    if (stack->left_canary_struct != DEFAULT_CANARY)   return error |= STACK_ERROR_LEFT_CANARY_STRUCT;
    if (stack->right_canary_struct != DEFAULT_CANARY)  return error |= STACK_ERROR_RIGHT_CANARY_STRUCT;
    int hash = get_hash(stack);
    if (stack->hash != hash)                           return error |= STACK_ERROR_HASH;
    if (*((long long *)stack->data + get_left_canary_index(stack)) != DEFAULT_CANARY)  return error |= STACK_ERROR_LEFT_CANARY_DATA;
    if (*((long long *)stack->data + get_right_canary_index(stack)) != DEFAULT_CANARY)  return error |= STACK_ERROR_RIGHT_CANARY_DATA;
    return error; 
}

static void print_error(int error, const char *s)
{
    fprintf(stderr, print_lred("ERROR: %d: %s\n"), error, s);
}

void print_stack_pointers(const Stack *stack)
{
    if (stack)                      printf(print_lyellow("stack =     %p\n"),   &stack);
    else return;
    if (stack->left_canary_struct)  printf(print_lyellow("left_can =  %p\n"),   &stack->left_canary_struct);
    if (stack->data)                printf(print_lyellow("data =      %p\n"),   &stack->data);
    if (stack->size)                printf(print_lyellow("size =      %p\n"),   &stack->size);
    if (stack->capacity)            printf(print_lyellow("capacity =  %p\n"),   &stack->capacity);
    if (stack->hash)                printf(print_lyellow("hash =      %p\n"),   &stack->hash);
    if (stack->right_canary_struct) printf(print_lyellow("right_can = %p\n"),   &stack->right_canary_struct);
}

static int powf(int x, int st)
{
    if (st == 0) return 1;
    if (st % 2 == 1) return x * powf(x, st - 1);
    int z = powf(x, st / 2);
    return z * z;
}

int long long make_number_canary()
{
    srand((unsigned int)time(NULL));
    return (rand() % 555 + 35 + abs(atoi("CHE") % 37)) * (rand() % 1000 + 35 + abs(atoi("ABOBA") % 555)) * (rand() % 10000 + 7777) * (rand() % 909 + 35 + abs(atoi("777")));
}

int get_hash(Stack *stack)
{
    int size_struct = sizeof(long long) + sizeof(type_el *) + sizeof(int) * 2;
    return calculate_hash(stack, size_struct) + 
           calculate_hash(stack->data, (int)sizeof(long long) * 2 + (int)sizeof(type_el) * stack->capacity);
}

static int calculate_hash(void *data, int size)
{
    int hash = 777;
    const int base = 31;
    const int mod = 1e9 + 7;

    for (int i = 0; i < size; i++)
        hash = ((hash * base) % mod + (*((char *)data + i)) * base) % mod;

    return hash;
}
