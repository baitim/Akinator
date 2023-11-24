#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Input.h"
#include "Tree.h"
#include "Output.h"

int main()
{
    printf(print_lblue("# Implementation of Tree.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));
    
    ErrorCode err = ERROR_NO;
    int number_graph_dump = 1;
    Tree *tree = nullptr;
    const char name_data_file[] = "tree.txt";

    err = tree_read(&tree, name_data_file);
    if (err) {
        err_dump(err);
        return err;
    }

    process_input(tree);

    err = tree_graph_dump(tree, &number_graph_dump);
    if (err) {
        err_dump(err);
        return err;
    }

    FILE *data_file = fopen(name_data_file, "w");
    if (!data_file) {
        err = ERROR_OPEN_FILE;
        err_dump(err);
        return err;
    }
    err = tree_write(&tree, data_file, 0);
    if (err) {
        err_dump(err);
        return err;
    }
    fclose(data_file);

    err = tree_delete(tree);
    if (err) {
        err_dump(err);
        return err;
    }

    err = tree_html_dump(number_graph_dump);
    if (err) {
        err_dump(err);
        return err;
    }

    printf(print_lblue("\nBye\n"));

    return 0;
}