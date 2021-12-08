#include "menu.h"

#include "error.h"

#include <stdlib.h>
#include <string.h>

typedef void (*TrieFunc)(void *userdata);

typedef struct Trie {
    TrieFunc func;
    char *word;
    struct Trie *branches[26];
} Trie;

struct MenuData {
    Trie *root;
};

static Trie *trie_create() {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    trie->word = NULL;
    trie->func = NULL;
    memset(trie->branches, 0, sizeof(trie->branches));
    return trie;
}

static void trie_free(Trie *trie) {
    for (size_t i = 0; i < 26; i++) {
        if (trie->branches[i] != NULL) {
            trie_free(trie->branches[i]);
        }
    }
    free(trie->word);
    free(trie);
}

static void trie_add(Trie *root, char *word, TrieFunc func) {
    char *wordPtr = word;
    while (*wordPtr != '\0') {
        if (root->branches[*wordPtr - 'a'] == NULL) {
            if (root->word == NULL) {
            } else {
            }
        } else {
            root = root->branches[*wordPtr - 'a'];
            wordPtr++;
        }
    }

    if (root->word != NULL) {
        eprintf("Word was already assigned as a menu: %s", word);
        exit(-1);
    }

    root->word = (char *)malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(root->word, word);
    root->func = func;
}

MenuData *menu_create() {
    MenuData *menu = (MenuData *)malloc(sizeof(MenuData));
    menu->root = trie_create();
    return menu;
}

void menu_free(MenuData *menu) {
    trie_free(menu->root);
    free(menu);
}

void menu_add(char *menu, TrieFunc func) {
}
