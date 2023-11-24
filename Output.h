#ifndef OUTPUT_H
#define OUTPUT_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_cmd_dump     (Tree *tree);
ErrorCode tree_graph_dump   (Tree *tree, int *number_graph_dump);
ErrorCode tree_html_dump    (int number_graph_dump);
ErrorCode tree_write        (Tree **tree, FILE *data_file, int dep);

#endif // OUTPUT_H