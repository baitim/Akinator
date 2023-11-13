#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Tree.h"

int main()
{
    printf(print_lblue("# Implementation of Tree.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));
    
    Type_Error err = ERROR_NO;

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
    err = tree_cmd_dump(tree);
    if (err) {
        err_dump(err);
        return err;
    }

    fclose(data_file);

    char *object = nullptr;
    size_t len_str = 0;
    for (int i = 0; i < 2; i++) {
        printf(print_lcyan("Input name of object:\n"));
        int read_count = (int)getline(&object, &len_str, stdin);
        object[read_count - 1] = '\0';
        if (read_count == -1) {
            printf("Cannot read input\n");
            continue;
        }
        err = tree_insert(&tree, object);
        if (err) {
            err_dump(err);
            return err;
        }
        err = tree_cmd_dump(tree);
        if (err) {
            err_dump(err);
            return err;
        }
    }
    free(object);

    data_file = fopen("tree.txt", "w");
    if (!data_file) {
        printf("Error open file with tree data\n");
        return 1;
    }

    err = tree_write(&tree, data_file);
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