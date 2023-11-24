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
ErrorCode tree_delete       (Tree *tree);
ErrorCode tree_verify       (Tree *tree);

#endif // TREE_H