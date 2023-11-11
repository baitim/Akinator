#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "Errors.h"

struct Tree {
    int data;
    Tree *left;
    Tree *right;
};

Type_Error tree_new         (Tree **tree, int value);
Type_Error tree_delete      (Tree *tree);
Type_Error tree_cmd_dump    (Tree *tree);
Type_Error tree_graph_dump  (Tree *tree, FILE *dump_file);
Type_Error tree_verify      (Tree *tree);
Type_Error tree_insert      (Tree **tree, int value);

#endif // TREE_H