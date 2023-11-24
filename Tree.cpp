#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ANSI_colors.h"
#include "Tree.h"
#include "Stack/Stack.h"
#include "Input.h"

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

ErrorCode tree_verify(Tree *tree)
{
    assert(tree);

    if (!tree->data) return ERROR_EMPTY_NODE;
    if (tree->left)  return tree_verify(tree->left);
    if (tree->right) return tree_verify(tree->right);
    
    return ERROR_NO;
}
