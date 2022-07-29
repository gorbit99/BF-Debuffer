#pragma once

typedef struct MenuData MenuData;

MenuData *menu_create(int argc, char **argv);

void menu_free(MenuData *menu);

void menu_run(MenuData *menu);
