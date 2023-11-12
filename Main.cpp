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

    char object[MAX_SIZE_DATA] = "";
    for (int i = 0; i < 5; i++) {
        printf(print_lcyan("Input name of object:\n"));
        int read_count = scanf("%s", object);
        if (read_count != 1) {
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