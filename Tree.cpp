#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ANSI_colors.h"
#include "Tree.h"
#include "Stack/Stack.h"

const int MAX_SIZE_NAME_DUMP = 50;
const int MAX_SIZE_COMMAND = 100;

static ErrorCode tree_cmd_dump_(Tree *tree, int dep);
static ErrorCode tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep);
static ErrorCode tree_graph_dump_make_edge(Tree *tree, FILE *dump_file);
static void get_input_word(char *str, FILE *data_file);
static ErrorCode tree_init(Tree **tree);
static ErrorCode get_object(char **object);
static ErrorCode tree_get_description(Tree *tree, Stack *stack, char *s, int *is_found);
static ErrorCode tree_print_description(Tree *tree, Stack *stack);
static ErrorCode tree_print_difference(Tree *tree, Stack *stack1, Stack *stack2);

enum Branch {
    BRANCH_LEFT =  -1,
    BRANCH_RIGHT =  1,
};

ErrorCode tree_new(Tree **tree, char *str)
{
    if (!tree && !*tree) return ERROR_INVALID_TREE;

    tree_insert(tree, str);

    return tree_verify((*tree));
}

ErrorCode tree_delete(Tree *tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left) return tree_delete(tree->left);
    if (tree->right) return tree_delete(tree->right);

    free(tree);
    
    return ERROR_NO;
}

ErrorCode tree_cmd_dump(Tree *tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    printf(print_magenta("^^^^^^^^^^^^^^^^^^^^\n\n"));
    printf(print_magenta("Tree:\n"));

    tree_cmd_dump_(tree, 0);

    printf(print_magenta("vvvvvvvvvvvvvvvvvvvv\n\n"));

    return tree_verify(tree);
}

static ErrorCode tree_cmd_dump_(Tree *tree, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left) tree_cmd_dump_(tree->left, dep + 1);

    for (int i = 0; i < dep; i++) printf("\t");
    if (!tree->left && !tree->right)    printf(print_lgreen("%s\n"), tree->data);
    else                                printf(print_lyellow("%s\n"), tree->data);

    if (tree->right) tree_cmd_dump_(tree->right, dep + 1);

    return tree_verify(tree);
}

ErrorCode tree_graph_dump(Tree *tree, int *number_graph_dump)
{
    char buffer[MAX_SIZE_NAME_DUMP] = "";
    snprintf(buffer, MAX_SIZE_NAME_DUMP, "dumps/dump%d", *number_graph_dump);

    FILE *dump_file = fopen(buffer, "w");
    if (!dump_file) {
        printf("Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    if (!tree)
        return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    fprintf(dump_file, "digraph {\n"
                       "\tgraph[label = \"Tree\", labelloc = top, "
                       "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n"
                       "\tgraph[dpi = 100];\n"
                       "\tbgcolor = \"#2F353B\";\n"
                       "\tedge[minlen = 3, arrowsize = 1.5, penwidth = 3];\n"
                       "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, "
                       "fillcolor = \"#ab5b0f\", fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n");

    err = tree_graph_dump_make_node(tree, dump_file, 1);
    if (err) return err;

    err = tree_graph_dump_make_edge(tree, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");

    fclose(dump_file);

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "gvpack -u %s | dot -Tpng -o %s.png", buffer, buffer);
    int sys = system(command);
    if (sys) 
        return ERROR_SYSTEM_COMMAND;

    (*number_graph_dump)++;
    return tree_verify(tree);
}

ErrorCode tree_html_dump(int number_graph_dump)
{
    char buffer[MAX_SIZE_NAME_DUMP] = "";
    snprintf(buffer, MAX_SIZE_NAME_DUMP, "dumps/dump.html");

    FILE *html_file = fopen(buffer, "w");
    if (!html_file) {
        printf("Error open file to dump\n");
        return ERROR_OPEN_FILE;
    }

    fprintf(html_file, "<pre>\n");

    for (int i = 1; i <= number_graph_dump; i++) {
        fprintf(html_file, "<img src = \"dump%d.png\">\n", i);
    }

    fprintf(html_file, "</pre>\n");

    fclose(html_file);
    return ERROR_NO;
}

static ErrorCode tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
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

static ErrorCode tree_graph_dump_make_edge(Tree *tree, FILE *dump_file)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left)  tree_graph_dump_make_edge(tree->left, dump_file);
    if (tree->left)  fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->left);
    if (tree->right) fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->right);
    if (tree->right) tree_graph_dump_make_edge(tree->right, dump_file);

    return tree_verify(tree);
}

ErrorCode tree_verify(Tree *tree)
{
    assert(tree);

    if (!tree->data) return ERROR_EMPTY_NODE;
    if (tree->left)  return tree_verify(tree->left);
    if (tree->right) return tree_verify(tree->right);
    
    return ERROR_NO;
}

ErrorCode tree_read(Tree **tree, FILE *data_file)
{
    assert(data_file);

    int t = 0;
    while (isspace(t = getc(data_file)))
        ;

    ErrorCode err = ERROR_NO;
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

ErrorCode tree_write(Tree **tree, FILE *data_file, int dep)
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

static ErrorCode tree_init(Tree **tree)
{
    *tree = (Tree *)calloc(1, sizeof(Tree));
    if (!*tree) return ERROR_ALLOC_FAIL;

    (*tree)->data = (char *)calloc(MAX_SIZE_DATA, sizeof(char));
    if (!(*tree)->data) return ERROR_ALLOC_FAIL; 

    return ERROR_NO;
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