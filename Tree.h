#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "Errors.h"

const int MAX_SIZE_DATA = 100;

struct Tree {
    char *data;
    Tree *left;
    Tree *right;
};

Type_Error tree_new         (Tree **tree, char *str);
Type_Error tree_delete      (Tree *tree);
Type_Error tree_cmd_dump    (Tree *tree);
Type_Error tree_graph_dump  (Tree *tree, FILE *dump_file);
Type_Error tree_verify      (Tree *tree);
Type_Error tree_insert      (Tree **tree, char *str);

#endif // TREE_H