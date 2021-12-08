#pragma once

typedef struct MenuData MenuData;

MenuData *menu_create();

void menu_free(MenuData *menu);

void menu_add(char *menu, void (*func)(void *userdata));
