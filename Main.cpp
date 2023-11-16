#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Input.h"
#include "Tree.h"

int main()
{
    printf(print_lblue("# Implementation of Tree.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));
    
    TypeError err = ERROR_NO;

    FILE *dump_file = fopen("dump.dot", "w");
    if (!dump_file) {
        printf("Error open file to dump\n");
        return 1;
    }

    Tree *tree = nullptr;

    FILE *data_file = fopen("tree.txt", "r");
    if (!data_file) {
        printf("Error open file with tree data\n");
        return 1;
    }
    err = tree_read(&tree, data_file);
    if (err) {
        err_dump(err);
        return err;
    }
    fclose(data_file);

    process_input(tree);

    data_file = fopen("tree.txt", "w");
    if (!data_file) {
        printf("Error open file with tree data\n");
        return 1;
    }
    err = tree_write(&tree, data_file, 0);
    if (err) {
        err_dump(err);
        return err;
    }
    fclose(data_file);

    err = tree_graph_dump(tree, dump_file);
    if (err) {
        err_dump(err);
        return err;
    }

    err = tree_delete(tree);
    if (err) {
        err_dump(err);
        return err;
    }

    fclose(dump_file);

    printf(print_lblue("\nBye\n"));

    return 0;
}