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

TypeError tree_new         (Tree **tree, char *str);
TypeError tree_read        (Tree **tree, FILE *data_file);
TypeError tree_write       (Tree **tree, FILE *data_file, int dep);
TypeError tree_delete      (Tree *tree);
TypeError tree_cmd_dump    (Tree *tree);
TypeError tree_graph_dump  (Tree *tree, FILE *dump_file);
TypeError tree_verify      (Tree *tree);
TypeError tree_insert      (Tree **tree, char *str);

#endif // TREE_H