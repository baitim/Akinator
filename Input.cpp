#include <stdio.h>
#include <stdlib.h>

#include "ANSI_colors.h"
#include "Input.h"

static TypeError input_object(Tree *tree);

TypeError process_input(Tree *tree)
{
    TypeError err = ERROR_NO;

    while (true) {
        int c;

        printf("Choose action:\n"
               "\tG - guess object\n"
               "\tP - print tree\n"
               "\tE - exit\n");

        c = getc(stdin);

        switch (c) {
            case 'G':
                err = input_object(tree);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'P':
                err = tree_cmd_dump(tree);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'E':
                return ERROR_NO;

            default:
                printf(print_lred("Wrong input\n"));
                break;
        }
        getc(stdin);
    }

    return err;
}

static TypeError input_object(Tree *tree)
{
    printf(print_lcyan("Input name of object:\n"));
    char *object = nullptr;
    size_t len_str = 0;
    int read_count = (int)getline(&object, &len_str, stdin);
    object[read_count - 1] = '\0';
    if (read_count == -1) {
        printf("Cannot read input\n");
        free(object);
        return ERROR_INVALID_INPUT;
    }
    TypeError err = tree_insert(&tree, object);
    free(object);
    return err;
}