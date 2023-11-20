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

ErrorCode tree_new          (Tree **tree, char *str);
ErrorCode tree_read         (Tree **tree, FILE *data_file);
ErrorCode tree_write        (Tree **tree, FILE *data_file, int dep);
ErrorCode tree_delete       (Tree *tree);
ErrorCode tree_cmd_dump     (Tree *tree);
ErrorCode tree_graph_dump   (Tree *tree, int *number_graph_dump);
ErrorCode tree_html_dump    (int number_graph_dump);
ErrorCode tree_verify       (Tree *tree);
ErrorCode tree_insert       (Tree **tree, char *str);
ErrorCode tree_description  (Tree *tree);
ErrorCode tree_compare      (Tree *tree);

#endif // TREE_H