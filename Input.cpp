#include <stdio.h>
#include <stdlib.h>

#include "ANSI_colors.h"
#include "Input.h"

TypeError process_input(Tree *tree)
{
    TypeError err = ERROR_NO;

    while (true) {
        int c;

        printf(print_lcyan("Choose action:\n"
                           "\tG - guess object\n"
                           "\tP - print tree\n"
                           "\tD - print description of object\n"
                           "\tE - exit\n"));

        c = getc(stdin);
        getc(stdin);

        switch (c) {
            case 'G':
                err = tree_insert(&tree, nullptr);
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

            case 'D':
                err = tree_description(tree);
                if (err) {
                    err_dump(err);
                    return err;
                }
                break;

            case 'E':
                return ERROR_NO;

            default:
                getc(stdin);
                printf(print_lred("Wrong input\n"));
                break;
        }
    }

    return err;
}