#include "menu.h"

#include "error.h"
#include "inputstream.h"
#include "interpreter.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MenuData {
    BrainfuckInterpreter *interpreter;
    bool input_from_file;
};

static void print_help(char *program_name) {
    printf("%s <file>\n", program_name);
    printf("\t-r\t\t - Run only\n");
    printf("\t-c <output-file> - Compile to standard brainfuck\n");
    printf("\t-i <input-file>\t - Set file as input\n");
}

char *read_from_stdin(void *data) {
    (void)data;
    char *input = (char *)malloc(sizeof(char) * 500);
    input[0] = '\0';
    if (scanf("%[^\n]%*c", input) > 0) {
        strcat(input, "\n");
    } else if (scanf("%*c") != EOF) {
        strcat(input, "\n");
    }
    return input;
}

char *read_from_file(void *data) {
    FILE *file = (FILE *)data;

    char *input = (char *)malloc(sizeof(char) * 500);
    input[0] = '\0';
    if (fscanf(file, "%[^\n]%*c", input) > 1) {
        strcat(input, "\n");
    } else if (fscanf(file, "%*c") != EOF) {
        strcat(input, "\n");
    }
    return input;
}

MenuData *menu_create(int argc, char **argv) {
    if (argc == 1) {
        print_help(argv[0]);
        return NULL;
    }

    char *file_name = NULL;
    char *input_file_name = NULL;
    bool run_immediately = false;
    char *write_out = NULL;

    for (int i = 1; i < argc; i++) {
        char *cur_arg = argv[i];

        if (cur_arg[0] == '-') {
            if (strcmp(cur_arg, "-i") == 0) {
                if (argc == i) {
                    eprintf("Expected argument after %s!", cur_arg);
                    exit(-1);
                }
                if (input_file_name != NULL) {
                    eprintf("Duplicate %s!", cur_arg);
                    exit(-1);
                }
                input_file_name = argv[i + 1];
                i++;
            } else if (strcmp(cur_arg, "-r") == 0) {
                if (run_immediately) {
                    eprintf("Duplicate %s!", cur_arg);
                    exit(-1);
                }
                run_immediately = true;
            } else if (strcmp(cur_arg, "-c") == 0) {
                if (argc == i) {
                    eprintf("Expected argument after %s!", cur_arg);
                    exit(-1);
                }
                if (write_out != NULL) {
                    eprintf("Duplicate %s!", cur_arg);
                    exit(-1);
                }
                write_out = argv[i + 1];
                i++;
            }
        } else {
            file_name = cur_arg;
        }
    }

    if (file_name == NULL) {
        eprintf("Input file not specified!\n", 1);
        exit(-1);
    }

    InputStream *inputstream;
    if (input_file_name) {
        FILE *input_file = fopen(input_file_name, "rt");
        inputstream = inputstream_create(read_from_file, input_file);
    } else {
        inputstream = inputstream_create(read_from_stdin, NULL);
    }

    BrainfuckInterpreter *interpreter =
            interpreter_create(file_name, inputstream);

    if (write_out != NULL) {
        interpreter_write_code(interpreter, write_out);
        return NULL;
    }

    if (run_immediately) {
        interpreter_run(interpreter, true);
        interpreter_print_output(interpreter);
        return NULL;
    }

    MenuData *menu = (MenuData *)malloc(sizeof(MenuData));
    menu->interpreter = interpreter;
    menu->input_from_file = input_file_name != NULL;

    return menu;
}

void menu_free(MenuData *menu) {
    interpreter_free(menu->interpreter);
    if (menu->input_from_file) {
        InputStream *is = interpreter_get_inputstream(menu->interpreter);
        fclose(inputstream_get_data(is));
    }
    free(menu);
}

static void print_commands() {
    printf("\t%-30s - Print this help menu\n", "h[elp]");
    printf("\t%-30s - Step execution forward\n", "s[tep]");
    printf("\t%-30s - Continue execution until next break\n", "c[ontinue]");
    printf("\t%-30s - Quit the debugger\n", "q[uit]");
    printf("\t%-30s - Optimize +-<>\n", "o[ptimize] <true/false>");
    printf("\t%-30s - Execute code\n", "e[xecute] <code>");
    printf("\t%-30s - Update code\n", "u[pdate]");
    printf("\t%-30s - Set EOF mode\n",
           "f[ileending] <zero/negative/newline/unchanged>");
}

void menu_run(MenuData *menu) {
    bool quit = false;
    char prev[200] = "";
    bool print_help = false;
    while (!quit) {
        printf("\x1b[2J\x1b\x1b[H");
        interpreter_print_code(menu->interpreter);
        printf("\n");
        interpreter_print_context(menu->interpreter);
        printf("\n");
        interpreter_print_output(menu->interpreter);

        if (print_help) {
            print_commands();
            print_help = false;
        }

        printf("\n>> ");

        char input[200];
        if (scanf("%200[^\n]", input) != 1) {
            strcpy(input, prev);
        }
        scanf("%*c");

        char command[50];
        int read;
        sscanf(input, "%s%n", command, &read);
        char *args = input + read;

        if (command[0] == 'h') {
            print_help = true;
        } else if (command[0] == 'q') {
            quit = true;
        } else if (command[0] == 'c') {
            interpreter_run(menu->interpreter, false);
        } else if (command[0] == 's') {
            interpreter_step(menu->interpreter);
        } else if (command[0] == 'o') {
            char bool_arg[10];
            sscanf(args, "%s", bool_arg);
            bool res;
            if (bool_arg[0] == 't') {
                res = true;
            } else if (bool_arg[0] == 'f') {
                res = false;
            } else {
                printf("Invalid argument!\n");
                continue;
            }

            interpreter_set_optimized(menu->interpreter, res);
        } else if (command[0] == 'e') {
            while (*args != '\0') {
                interpreter_exec_char(menu->interpreter, *args);
                args++;
            }
        } else if (command[0] == 'u') {
            interpreter_read_code(menu->interpreter);

        } else if (command[0] == 'f') {
            char arg[20];
            sscanf(args, "%s", arg);
            EOFMode mode = EOF_Negative;
            if (strcmp(arg, "zero") == 0) {
                mode = EOF_Zero;
            } else if (strcmp(arg, "negative") == 0) {
                mode = EOF_Negative;
            } else if (strcmp(arg, "newline") == 0) {
                mode = EOF_Newline;
            } else if (strcmp(arg, "unchanged") == 0) {
                mode = EOF_Unchanged;
            }
            interpreter_set_eof_mode(menu->interpreter, mode);
        } else {
            printf("Invalid command!\n");
            continue;
        }
        strcpy(prev, command);
    }
}
