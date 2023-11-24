#ifndef INPUT_H
#define INPUT_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_read         (Tree **tree, const char *data_file);
ErrorCode process_input     (Tree *tree);
ErrorCode tree_insert       (Tree **tree, char *str);
ErrorCode tree_description  (Tree *tree);
ErrorCode tree_compare      (Tree *tree);
ErrorCode tree_read         (Tree **tree, FILE *data_file);

#endif // INPUT_H