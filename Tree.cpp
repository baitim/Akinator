#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tree.h"

const int MAX_SIZE_DATA = 100;

static Type_Error tree_cmd_dump_(Tree *tree, int dep);
static Type_Error tree_graph_dump_make_node(Tree *tree, FILE *dump_file, int dep);
static Type_Error tree_graph_dump_make_edge(Tree *tree, FILE *dump_file);

Type_Error tree_new(Tree **tree, int value)
{
    if (!tree)
        return ERROR_INVALID_TREE;

    tree_insert(tree, value);

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

    printf("^^^^^^^^^^^^^^^^^^^^\n\n");
    printf("Tree:\n");

    tree_cmd_dump_(tree, 0);

    printf("vvvvvvvvvvvvvvvvvvvv\n\n");

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
    printf("%d\n", tree->data);

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

    fprintf(dump_file, "\t\tnode%p[label = \"{ %d | {", tree, tree->data);
    if (tree->left) fprintf(dump_file, "%d", tree->left->data);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, " | ");
    if (tree->right)fprintf(dump_file, "%d", tree->right->data);
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

    if (tree->left)
        return tree_verify(tree->left);

    if (tree->right)
        return tree_verify(tree->right);
    
    return ERROR_NO;
}

Type_Error tree_insert(Tree **tree, int value)
{
    if (!*tree) {
        *tree = (Tree *)malloc(sizeof(Tree));
        if (!*tree)
            return ERROR_ALLOC_FAIL;

        **tree = (Tree){value, nullptr, nullptr};

        return ERROR_NO;
    }

    Type_Error err = tree_verify(*tree);
    if (err) return err;

    if (value <= (*tree)->data) { 
        tree_insert(&(*tree)->left, value);
    } else {
        tree_insert(&(*tree)->right, value);
    }
    return tree_verify(*tree);
}