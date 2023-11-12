#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Tree.h"

static Type_Error tree_cmd_dump_(Tree *tree, int dep);
static Type_Error tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep);
static Type_Error tree_graph_dump_make_edge(Tree *tree, FILE *dump_file);

Type_Error tree_new(Tree **tree, char *str)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    tree_insert(tree, str);

    return tree_verify((*tree));
}

Type_Error tree_delete(Tree *tree)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    Type_Error err = tree_verify(tree);
    if (err) return err;

    if (tree->left)
        return tree_delete(tree->left);

    if (tree->right)
        return tree_delete(tree->right);

    free(tree);
    
    return ERROR_NO;
}

Type_Error tree_cmd_dump(Tree *tree)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    Type_Error err = tree_verify(tree);
    if (err) return err;

    printf(print_lgreen("^^^^^^^^^^^^^^^^^^^^\n\n"));
    printf(print_lgreen("Tree:\n"));

    tree_cmd_dump_(tree, 0);

    printf(print_lgreen("vvvvvvvvvvvvvvvvvvvv\n\n"));

    return tree_verify(tree);
}

static Type_Error tree_cmd_dump_(Tree *tree, int dep)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    Type_Error err = tree_verify(tree);
    if (err) return err;

    if (tree->left)
        tree_cmd_dump_(tree->left, dep + 1);

    for (int i = 0; i < dep; i++) printf("\t");
    printf(print_lgreen("%s\n"), tree->data);

    if (tree->right)
        tree_cmd_dump_(tree->right, dep + 1);

    return tree_verify(tree);
}

Type_Error tree_graph_dump(Tree *tree, FILE *dump_file)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    if (!dump_file)
        return ERROR_INVALID_FILE;

    Type_Error err = tree_verify(tree);
    if (err) return err;

    fprintf(dump_file, "digraph {\n");
    fprintf(dump_file, "\tgraph[label = \"Tree\", labelloc = top, ");
    fprintf(dump_file, "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n");
    fprintf(dump_file, "\tgraph[dpi = 200];\n");
    fprintf(dump_file, "\tbgcolor = \"#2F353B\";\n");
    fprintf(dump_file, "\tedge[minlen = 3, arrowsize = 1.5, penwidth = 3];\n");

    fprintf(dump_file, "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, ");
	fprintf(dump_file, "fillcolor = \"#ab5b0f\", fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n");

    err = tree_graph_dump_make_node(tree, dump_file, 1);
    if (err) return err;

    err = tree_graph_dump_make_edge(tree, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");

    return tree_verify(tree);
}

static Type_Error tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    Type_Error err = tree_verify(tree);
    if (err) return err;

    if (tree->left)
        tree_graph_dump_make_node(tree->left, dump_file, dep + 1);

    fprintf(dump_file, "\t{ \n");
    fprintf(dump_file, "\t\tnode[shape = \"Mrecord\"];\n");

    fprintf(dump_file, "\t\tnode%p[label = \"{ %s | {", tree, tree->data);
    if (tree->left) fprintf(dump_file, "%s", tree->left->data);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, " | ");
    if (tree->right)fprintf(dump_file, "%s", tree->right->data);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, "} }\"];\n");

    fprintf(dump_file, "\t}\n");

    if (tree->right)
        tree_graph_dump_make_node(tree->right, dump_file, dep + 1);

    return tree_verify(tree);
}

static Type_Error tree_graph_dump_make_edge(Tree *tree, FILE *dump_file)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    Type_Error err = tree_verify(tree);
    if (err) return err;

    if (tree->left)
        tree_graph_dump_make_edge(tree->left, dump_file);

    if (tree->left) 
        fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->left);

    if (tree->right) 
        fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->right);

    if (tree->right)
        tree_graph_dump_make_edge(tree->right, dump_file);

    return tree_verify(tree);
}

Type_Error tree_verify(Tree *tree)
{
    assert(tree);

    if (!tree->data)
        return ERROR_EMPTY_NODE;

    if (tree->left)
        return tree_verify(tree->left);

    if (tree->right)
        return tree_verify(tree->right);
    
    return ERROR_NO;
}

Type_Error tree_insert(Tree **tree, char *str)
{
    if (!*tree) {
        *tree = (Tree *)malloc(sizeof(Tree));
        if (!*tree)
            return ERROR_ALLOC_FAIL;

        **tree = (Tree){nullptr, nullptr, nullptr};

        (*tree)->data = (char *)calloc(MAX_SIZE_DATA, sizeof(char));
        if (!(*tree)->data)
            return ERROR_ALLOC_FAIL;

        strcpy((*tree)->data, str);

        return ERROR_NO;
    }

    Type_Error err = tree_verify(*tree);
    if (err) return err;

    char answer[MAX_SIZE_DATA] = "";
    int read_count = 0;

    if (!(*tree)->left && !(*tree)->right) {
        printf("What is difference between %s and %s?\n", (*tree)->data, str);
        read_count = scanf("%s", answer);
        if (read_count != 1)
            return ERROR_READ_INPUT;

        tree_insert(&(*tree)->left, (*tree)->data);
        tree_insert(&(*tree)->right, str);
        strcpy((*tree)->data, answer);

        return tree_verify(*tree);
    }

    printf(print_lcyan("Is %s?\n"), (*tree)->data);
    read_count = scanf("%s", answer);
    if (read_count != 1)
        return ERROR_READ_INPUT;

    if (strncmp(answer, "yes", 3) == 0 || strncmp(answer, "да", 3) == 0) { 
        tree_insert(&(*tree)->left, str);
    } else if (strncmp(answer, "no", 2) == 0  || strncmp(answer, "нет", 3) == 0) {
        tree_insert(&(*tree)->right, str);
    } else {
        return ERROR_READ_INPUT;
    }

    return tree_verify(*tree);
}