#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ANSI_colors.h"
#include "Input.h"
#include "Output.h"
#include "Stack/Stack.h"

enum Branch {
    BRANCH_LEFT =  -1,
    BRANCH_RIGHT =  1,
};

static void get_input_word(char *str, char** buf);
static ErrorCode tree_read_(Tree **tree, char** buf);
static ErrorCode tree_init(Tree **tree);
static ErrorCode get_object(char **object);
static ErrorCode tree_get_description(Tree *tree, Stack *stack, char *s, int *is_found);
static ErrorCode tree_print_description(Tree *tree, Stack *stack);
static ErrorCode tree_print_difference(Tree *tree, Stack *stack1, Stack *stack2);
static ErrorCode file_to_buf(const char* name_file, char** buf);
static ErrorCode fsize(const char* name_file, int* size_file);
static char* skip_spaces(char* s);
static char* skip_word  (char* s);

ErrorCode process_input(Tree *tree)
{
    ErrorCode err = ERROR_NO;

    while (true) {
        int c = -1;

        printf(print_lcyan("Choose action:\n"
                           "\tG - guess object\n"
                           "\tP - print tree\n"
                           "\tD - print description of object\n"
                           "\tC - compare two objects\n"
                           "\tE - exit\n"));

        c = getc(stdin);
        getc(stdin);

        switch (c) {
            case 'G':
                err = tree_insert(&tree, nullptr);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'P':
                err = tree_cmd_dump(tree);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'D':
                err = tree_description(tree);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'C':
                err = tree_compare(tree);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'E':
                return ERROR_NO;

            default:
                getc(stdin);
                printf(print_lred("Wrong input\n"));
                break;
        }
    }

    return err;
}

static ErrorCode tree_init(Tree **tree)
{
    *tree = (Tree *)calloc(1, sizeof(Tree));
    if (!*tree) return ERROR_ALLOC_FAIL;

    (*tree)->data = (char *)calloc(MAX_SIZE_DATA, sizeof(char));
    if (!(*tree)->data) return ERROR_ALLOC_FAIL; 

    return ERROR_NO;
}

ErrorCode tree_read(Tree **tree, const char *data_file)
{
    assert(data_file);
    if (!tree) return ERROR_INVALID_TREE;

    char *buf = nullptr;
    ErrorCode err = file_to_buf(data_file, &buf);
    if (err) return err;

    err = tree_read_(tree, &buf);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode tree_read_(Tree **tree, char** buf)
{
    assert(buf);

    (*buf) = skip_spaces(*buf);

    ErrorCode err = ERROR_NO;
    if (**buf == '(') {
        tree_init(tree);
    
        (*buf)++;
        get_input_word((*tree)->data, buf);
        (*buf)++;

        err = tree_read_(&(*tree)->left, buf);
        if (err) return err;

        (*buf)++;
        err = tree_read_(&(*tree)->right, buf);
        if (err) return err;

        (*buf) = skip_spaces(*buf);
        (*buf)++;

        return tree_verify(*tree);
    } else {
        char nill[MAX_SIZE_DATA] = "";
        get_input_word(nill, buf);   
        return ERROR_NO;
    }

    return ERROR_NO;
}

static char* skip_spaces(char* s)
{
    while (*s != '\0' && isspace(*s))
        s++;
    return s;
}

static char* skip_word(char* s)
{
    while (*s != '\0' && !isspace(*s))
        s++;
    return s;
}

static void get_input_word(char *str, char **buf)
{
    (*buf)++;
    int i = 0;
    while (i < MAX_SIZE_DATA - 1 && (**buf) != '>') {
        str[i] = **buf;
        i++;
        (*buf)++;
    }
    (*buf)++;
    str[i + 1] = '\0';
}

ErrorCode file_to_buf(const char* name_file, char** buf)
{
    if (!name_file) return ERROR_OPEN_FILE;

    ErrorCode err = ERROR_NO;
    FILE* data_file = fopen(name_file, "r");
    if (!data_file) return ERROR_OPEN_FILE;

    int size_file = -1;
    err = fsize(name_file, &size_file);
    if (err) return err;

    *buf = (char *)calloc(size_file, sizeof(char));
    if (!(*buf)) return ERROR_ALLOC_FAIL;

    int count_read = (int)fread(*buf, sizeof(char), size_file, data_file);
    if (count_read != size_file - 1) return ERROR_READ_FILE;
    (*buf)[size_file - 1] = '\0';

    fclose(data_file);
    return err;
}

static ErrorCode fsize(const char* name_file, int* size_file) 
{
    assert(name_file);
    struct stat st = {};

    if (stat(name_file, &st) == 0) {
        (*size_file) = (int)st.st_size + 1;
        return ERROR_NO;
    }

    return ERROR_READ_FILE;
}

ErrorCode tree_insert(Tree **tree, char *str)
{
    ErrorCode err = ERROR_NO;
    if (!*tree) {
        err = tree_init(tree);
        if (err) return err;

        strcpy((*tree)->data, str);
        return ERROR_NO;
    }

    err = tree_verify(*tree);
    if (err) return err;

    int read_count = 0;
    size_t len_str = 1;
    char *question = nullptr;
    printf(print_lcyan("Is %s?\n"), (*tree)->data);
    read_count = (int)getline(&question, &len_str, stdin);
    question[read_count - 1] = '\0';
    if (read_count == -1)
        return ERROR_READ_INPUT;

    if (strncmp(question, "yes", 3) == 0 || strncmp(question, "да", 3) == 0) {

        if (!(*tree)->left && !(*tree)->right) {
            return ERROR_NO;
        } else {
            tree_insert(&(*tree)->left, str);
        }
    } else if (strncmp(question, "no", 2) == 0  || strncmp(question, "нет", 3) == 0) {

        if (!(*tree)->left && !(*tree)->right) {

            printf(print_lcyan("Input name of object:\n"));
            char *object = nullptr;
            read_count = (int)getline(&object, &len_str, stdin);
            object[read_count - 1] = '\0';
            if (read_count == -1) {
                printf("Cannot read input\n");
                free(object);
                return ERROR_INVALID_INPUT;
            }

            char *difference = nullptr;
            printf("What is difference between %s and %s?\n", (*tree)->data, object);
            read_count = (int)getline(&difference, &len_str, stdin);
            difference[read_count - 1] = '\0';
            if (read_count == -1)
                return ERROR_READ_INPUT;

            tree_insert(&(*tree)->left, (*tree)->data);
            tree_insert(&(*tree)->right, object);
            strcpy((*tree)->data, difference);

            free(difference);
            free(object);

            return tree_verify(*tree);

        } else {
            tree_insert(&(*tree)->right, str);
        }
    } else {
        return ERROR_READ_INPUT;
    }
    free(question);

    return tree_verify(*tree);
}

ErrorCode tree_description(Tree *tree)
{
    assert(tree);

    ErrorCode tree_err = ERROR_NO;

    StackErrors stk_err = STACK_ERROR_NO;
    Stack stack = {};

    stk_err = stack_ctor(&stack);
    if (stk_err) return ERROR_STACK;

    char *object = nullptr;
    tree_err = get_object(&object);
    if (tree_err) return tree_err;

    int is_found = -1;
    ErrorCode err = tree_get_description(tree, &stack, object, &is_found);
    if (err) return err;
    if (is_found == -1) {
        printf(print_lyellow("Cannot find object\n"));
        return ERROR_NO;
    }

    printf(print_lgreen("%s:\n"), object);
    tree_err = tree_print_description(tree, &stack);
    if (tree_err) return tree_err;
    printf("\n");

    stk_err = stack_dtor(&stack);
    if (stk_err) return ERROR_STACK;
    free(object);
    return ERROR_NO;
}

static ErrorCode get_object(char **object)
{
    int read_count = 0;
    size_t len_str = 0;
    printf(print_lcyan("Input name of object:\n"));
    read_count = (int)getline(object, &len_str, stdin);
    if (read_count == -1) {
        printf("Cannot read input\n");
        return ERROR_INVALID_INPUT;
    }
    (*object)[read_count - 1] = '\0';
    return ERROR_NO;
}

static ErrorCode tree_get_description(Tree *tree, Stack *stack, char *s, int *is_found)
{
    if (!tree) return ERROR_NO;

    if (!tree->left && !tree->right) {
        if (strcmp(tree->data, s) == 0) (*is_found) = 1;
        else                            (*is_found) = 0;

        return ERROR_NO;
    }

    int is_left = 0, is_right = 0;
    if (tree->left ) tree_get_description(tree->left,  stack, s, &is_left );
    if (tree->right) tree_get_description(tree->right, stack, s, &is_right);
    if (is_left && is_right)
        return ERROR_EQUAL_OBJECTS;

    int x = 0;
    if (is_left )   x = BRANCH_LEFT;
    if (is_right)   x = BRANCH_RIGHT;

    if (is_left || is_right) {
        stack_push(stack, x); 
        (*is_found) = 1;
    }

    return ERROR_NO;
}

static ErrorCode tree_print_description(Tree *tree, Stack *stack)
{
    if (!tree->left && !tree->right) return ERROR_NO;

    StackErrors stk_err = STACK_ERROR_NO;

    int way = 0;
    stk_err = stack_pop(stack, &way);
    if (stk_err) return ERROR_STACK;

    if (way == BRANCH_LEFT) {
        printf(print_lyellow("%s\t"), tree->data);
        tree_print_description(tree->left, stack);
    } else {
        assert(way == BRANCH_RIGHT);
        printf(print_lyellow("не %s\t"), tree->data);
        tree_print_description(tree->right, stack);
    }

    return ERROR_NO;
}

ErrorCode tree_compare(Tree *tree)
{
    assert(tree);

    ErrorCode err = ERROR_NO;
    StackErrors stk_err = STACK_ERROR_NO;

    Stack stack1 = {};
    Stack stack2 = {};

    stk_err = stack_ctor(&stack1);
    if (stk_err) return ERROR_STACK;
    stk_err = stack_ctor(&stack2);
    if (stk_err) return ERROR_STACK;

    char *object1 = nullptr;
    err = get_object(&object1);
    if (err) return err;
    char *object2 = nullptr;
    err = get_object(&object2);
    if (err) return err;

    int is_found = -1;
    err = tree_get_description(tree, &stack1, object1, &is_found);
    if (err) return err;
    if (is_found == -1) {
        printf(print_lyellow("Cannot find first object\n"));
        return ERROR_NO;
    }
    is_found = -1;
    err = tree_get_description(tree, &stack2, object2, &is_found);
    if (err) return err;
    if (is_found == -1) {
        printf(print_lyellow("Cannot find second object\n"));
        return ERROR_NO;
    }

    printf(print_lcyan("Compare ") print_lgreen("%s") print_lcyan(" and ") 
           print_lgreen("%s") print_lcyan(":\n"), object1, object2);
    err = tree_print_difference(tree, &stack1, &stack2);
    if (err) return err;
    printf("\n");

    stk_err = stack_dtor(&stack1);
    if (stk_err) return ERROR_STACK;
    stk_err = stack_dtor(&stack2);
    if (stk_err) return ERROR_STACK;
    free(object1);
    free(object2);
    return ERROR_NO;
}

static ErrorCode tree_print_difference(Tree *tree, Stack *stack1, Stack *stack2)
{
    assert(tree);
    assert(stack1);
    assert(stack2);
    ErrorCode err = ERROR_NO;
    Tree *new_tree = tree;
    int x1 = 0, x2 = 0;
    printf(print_lcyan("Similarity between objects:\n"));
    while (stack1->size > 0 && stack2->size > 0) {
        stack_pop(stack1, &x1);
        stack_pop(stack2, &x2);

        if (x1 != x2) break;

        if (x1 == BRANCH_LEFT) {
            printf(print_lyellow("%s\t"), new_tree->data);
            new_tree = new_tree->left;
        } else {
            printf(print_lyellow("не %s\t"), new_tree->data);
            new_tree = new_tree->right;
        }
    }
    printf("\n");

    printf(print_lcyan("Difference between objects, first object:\n"));
    Tree *tree1 = new_tree;
    if (x1 == BRANCH_LEFT) {
        printf(print_lyellow("%s\t"), new_tree->data);
        tree1 = tree1->left;
    }
    if (x1 == BRANCH_RIGHT) {
        printf(print_lyellow("не %s\t"), new_tree->data);
        tree1 = tree1->right;
    }
    err = tree_print_description(tree1, stack1);
    if (err) return err;
    printf("\n");

    printf(print_lcyan("Difference between objects, second object:\n"));
    Tree *tree2 = new_tree;
    if (x2 == BRANCH_LEFT) {
        printf(print_lyellow("%s\t"), new_tree->data);
        tree2 = tree2->left;
    }
    if (x2 == BRANCH_RIGHT) {
        printf(print_lyellow("не %s\t"), new_tree->data);
        tree2 = tree2->right;
    }
    err = tree_print_description(tree2, stack2);
    if (err) return err;
    printf("\n");

    return ERROR_NO;
}
