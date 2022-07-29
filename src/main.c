#include "./inputstream.h"
#include "interpreter.h"
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    MenuData *menu = menu_create(argc, argv);

    if (menu == NULL) {
        return 0;
    }

    menu_run(menu);

    menu_free(menu);
}
