#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ANSI_colors.h"
#include "Tree.h"
#include "Stack/Stack.h"

static TypeError tree_cmd_dump_(Tree *tree, int dep);
static TypeError tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep);
static TypeError tree_graph_dump_make_edge(Tree *tree, FILE *dump_file);
static void get_input_word(char *str, FILE *data_file);
static TypeError tree_init(Tree **tree);
static int tree_get_description(Tree *tree, Stack *stack, char *s);
static TypeError tree_print_description(Tree *tree, Stack *stack);


TypeError tree_new(Tree **tree, char *str)
{
    if (!tree && !*tree) return ERROR_INVALID_TREE;

    tree_insert(tree, str);

    return tree_verify((*tree));
}

TypeError tree_delete(Tree *tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    TypeError err = tree_verify(tree);
    if (err) return err;

    if (tree->left) return tree_delete(tree->left);
    if (tree->right) return tree_delete(tree->right);

    free(tree);
    
    return ERROR_NO;
}

TypeError tree_cmd_dump(Tree *tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    TypeError err = tree_verify(tree);
    if (err) return err;

    printf(print_magenta("^^^^^^^^^^^^^^^^^^^^\n\n"));
    printf(print_magenta("Tree:\n"));

    tree_cmd_dump_(tree, 0);

    printf(print_magenta("vvvvvvvvvvvvvvvvvvvv\n\n"));

    return tree_verify(tree);
}

static TypeError tree_cmd_dump_(Tree *tree, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;

    TypeError err = tree_verify(tree);
    if (err) return err;

    if (tree->left) tree_cmd_dump_(tree->left, dep + 1);

    for (int i = 0; i < dep; i++) printf("\t");
    if (!tree->left && !tree->right)    printf(print_lgreen("%s\n"), tree->data);
    else                                printf(print_lyellow("%s\n"), tree->data);

    if (tree->right) tree_cmd_dump_(tree->right, dep + 1);

    return tree_verify(tree);
}

TypeError tree_graph_dump(Tree *tree, FILE *dump_file)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    if (!dump_file)
        return ERROR_INVALID_FILE;

    TypeError err = tree_verify(tree);
    if (err) return err;

    fprintf(dump_file, "digraph {\n"
                       "\tgraph[label = \"Tree\", labelloc = top, "
                       "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n"
                       "\tgraph[dpi = 200];\n"
                       "\tbgcolor = \"#2F353B\";\n"
                       "\tedge[minlen = 3, arrowsize = 1.5, penwidth = 3];\n"
                       "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, "
                       "fillcolor = \"#ab5b0f\", fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n");

    err = tree_graph_dump_make_node(tree, dump_file, 1);
    if (err) return err;

    err = tree_graph_dump_make_edge(tree, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");

    return tree_verify(tree);
}

static TypeError tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;

    TypeError err = tree_verify(tree);
    if (err) return err;

    if (tree->left) tree_graph_dump_make_node(tree->left, dump_file, dep + 1);

    fprintf(dump_file, "\t{ \n"
                       "\t\tnode[shape = \"Mrecord\"];\n");

    fprintf(dump_file, "\t\tnode%p[label = \"{ %s | {", tree, tree->data);
    if (tree->left) fprintf(dump_file, "%s", tree->left->data);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, " | ");
    if (tree->right)fprintf(dump_file, "%s", tree->right->data);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, "} }\"];\n");

    fprintf(dump_file, "\t}\n");

    if (tree->right) tree_graph_dump_make_node(tree->right, dump_file, dep + 1);

    return tree_verify(tree);
}

static TypeError tree_graph_dump_make_edge(Tree *tree, FILE *dump_file)
{
    if (!tree) return ERROR_INVALID_TREE;

    TypeError err = tree_verify(tree);
    if (err) return err;

    if (tree->left)  tree_graph_dump_make_edge(tree->left, dump_file);
    if (tree->left)  fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->left);
    if (tree->right) fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->right);
    if (tree->right) tree_graph_dump_make_edge(tree->right, dump_file);

    return tree_verify(tree);
}

TypeError tree_verify(Tree *tree)
{
    assert(tree);

    if (!tree->data) return ERROR_EMPTY_NODE;
    if (tree->left) return tree_verify(tree->left);
    if (tree->right) return tree_verify(tree->right);
    
    return ERROR_NO;
}

TypeError tree_read(Tree **tree, FILE *data_file)
{
    assert(data_file);

    int t = 0;
    while (isspace(t = getc(data_file)))
        ;

    TypeError err = ERROR_NO;
    if (t == '(') {
        tree_init(tree);
    
        get_input_word((*tree)->data, data_file);
        t = getc(data_file);

        err = tree_read(&(*tree)->left, data_file);
        if (err) return err;

        t = getc(data_file);
        err = tree_read(&(*tree)->right, data_file);
        if (err) return err;

        while (isspace(t = getc(data_file)))
            ;

        return tree_verify(*tree);
    } else {
        char nill[MAX_SIZE_DATA] = "";
        get_input_word(nill, data_file);   
        return ERROR_NO;
    }

    return ERROR_NO;
}

TypeError tree_write(Tree **tree, FILE *data_file, int dep)
{
    assert(data_file);
    if (!*tree) return ERROR_INVALID_TREE;

    fprintf(data_file, "(");

    fprintf(data_file, "<%s> ", (*tree)->data);

    if ((*tree)->left && (*tree)->right) {
        fprintf(data_file, "\n");
        for (int i = 0; i <= dep; i++) fprintf(data_file, "\t");
    }

    if ((*tree)->left)  tree_write(&(*tree)->left, data_file, dep + 1);
    else                fprintf(data_file, "<nill>");

    if ((*tree)->right) {
        fprintf(data_file, "\n");
        for (int i = 0; i <= dep; i++) fprintf(data_file, "\t");
    } else {
        fprintf(data_file, " ");
    }

    if ((*tree)->left) {
        tree_write(&(*tree)->right, data_file, dep + 1);
        fprintf(data_file, "\n");
        for (int i = 0; i < dep; i++) fprintf(data_file, "\t");
        fprintf(data_file, ")");
    } else {
        fprintf(data_file, "<nill>)");
    }

    return tree_verify(*tree);
}

static void get_input_word(char *str, FILE *data_file)
{
    int c = getc(data_file);
    int i = 0;
    while (i < MAX_SIZE_DATA - 1 && (c = getc(data_file)) != '>') {
        str[i] = (char)c;
        i++;
    }
    str[i + 1] = '\0';
}

static TypeError tree_init(Tree **tree)
{
    *tree = (Tree *)malloc(sizeof(Tree));
    if (!*tree) return ERROR_ALLOC_FAIL;

    **tree = (Tree){nullptr, nullptr, nullptr};

    (*tree)->data = (char *)calloc(MAX_SIZE_DATA, sizeof(char));
    if (!(*tree)->data) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

TypeError tree_insert(Tree **tree, char *str)
{
    TypeError err = ERROR_NO;
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

TypeError tree_description(Tree *tree)
{
    assert(tree);

    TypeError tree_err = ERROR_NO;

    int read_count = 0;
    size_t len_str = 0;
    printf(print_lcyan("Input name of object:\n"));
    char *object = nullptr;
    read_count = (int)getline(&object, &len_str, stdin);
    object[read_count - 1] = '\0';
    if (read_count == -1) {
        printf("Cannot read input\n");
        free(object);
        return ERROR_INVALID_INPUT;
    }

    StackErrors stk_err = STACK_ERROR_NO;
    Stack stack = {};

    stk_err = stack_ctor(&stack);
    if (stk_err) return ERROR_STACK;

    tree_get_description(tree, &stack, object);

    printf(print_lgreen("%s:\n"), object);
    tree_err = tree_print_description(tree, &stack);
    if (tree_err) return tree_err;
    printf("\n");

    stack_dtor(&stack);
    if (stk_err) return ERROR_STACK;

    free(object);
    return ERROR_NO;
}

static int tree_get_description(Tree *tree, Stack *stack, char *s)
{
    if (!tree) return ERROR_NO;

    if (!tree->left && !tree->right) {
        if (strcmp(tree->data, s) == 0) return 1;
        else                            return 0;
    }

    int is_left = 0, is_right = 0;
    if (tree->left)     is_left  = tree_get_description(tree->left, stack, s);
    if (tree->right)    is_right = tree_get_description(tree->right, stack, s);

    int x = 0;
    if (is_left)    x = 1;
    if (is_right)   x = -1;

    if (is_left || is_right) stack_push(stack, x);

    return x != 0;
}

static TypeError tree_print_description(Tree *tree, Stack *stack)
{
    if (!tree->left && !tree->right) return ERROR_NO;

    StackErrors stk_err = STACK_ERROR_NO;

    int way = 0;
    stk_err = stack_pop(stack, &way);
    if (stk_err) return ERROR_STACK;

    if (way == 1) {
        printf(print_lyellow("%s\t"), tree->data);
        tree_print_description(tree->left, stack);
    } else {
        printf(print_lyellow("не %s\t"), tree->data);
        tree_print_description(tree->right, stack);
    }

    return ERROR_NO;
}