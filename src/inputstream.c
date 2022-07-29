#include "inputstream.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

const size_t BLOCK_SIZE = 100;

typedef struct InputStreamBlock {
    char *blockData;
    size_t used;
    size_t occupied;
    struct InputStreamBlock *next;
} InputStreamBlock;

struct InputStream {
    InputStreamBlock *head;
    InputStreamBlock *tail;
    char *(*gather_func)(void *data);
    void *data;
};

static void streamblock_free(InputStreamBlock *head) {
    while (head != NULL) {
        InputStreamBlock *temp = head->next;
        free(head->blockData);
        free(head);
        head = temp;
    }
}

static bool streamblock_read(InputStreamBlock **head,
                             InputStreamBlock **tail,
                             char *(*gather_func)(void *data),
                             void *is_data) {
    char *input = gather_func(is_data);
    if (input == NULL) {
        return false;
    }

    char *left = input;
    size_t rem_len = strlen(left);

    if (rem_len == 0) {
        return false;
    }

    if (*tail == NULL) {
        *tail = (InputStreamBlock *)malloc(sizeof(InputStreamBlock));
        (*tail)->blockData = (char *)malloc(sizeof(char) * (BLOCK_SIZE + 1));
        (*tail)->occupied = 0;
        (*tail)->used = 0;
        (*tail)->next = NULL;
        *head = *tail;
    }
    if ((*tail)->occupied != BLOCK_SIZE) {
        if (rem_len >= BLOCK_SIZE - (*tail)->occupied) {
            strncpy((*tail)->blockData, left, BLOCK_SIZE - (*tail)->occupied);
            left += BLOCK_SIZE - (*tail)->occupied;
            rem_len -= BLOCK_SIZE - (*tail)->occupied;
            (*tail)->occupied = BLOCK_SIZE;
        } else {
            strcpy((*tail)->blockData, left);
            (*tail)->occupied += rem_len;
            rem_len = 0;
        }
    }

    while (rem_len > 0) {
        InputStreamBlock *newBlock =
                (InputStreamBlock *)malloc(sizeof(InputStreamBlock));
        newBlock->blockData = (char *)malloc(sizeof(char) * (BLOCK_SIZE + 1));
        newBlock->next = NULL;
        newBlock->used = 0;
        if (rem_len > BLOCK_SIZE) {
            strncpy(newBlock->blockData, left, BLOCK_SIZE);
            newBlock->blockData[BLOCK_SIZE] = 0;
            rem_len -= BLOCK_SIZE;
            left += BLOCK_SIZE;
            newBlock->occupied = BLOCK_SIZE;
        } else {
            strcpy(newBlock->blockData, left);
            newBlock->occupied = rem_len;
            rem_len = 0;
        }
        (*tail)->next = newBlock;
        (*tail) = newBlock;
    }

    free(input);
    return true;
}

InputStream *inputstream_create(char *(*gather_func)(void *data), void *data) {
    InputStream *inputstream = (InputStream *)malloc(sizeof(InputStream));
    inputstream->data = data;
    inputstream->head = NULL;
    inputstream->tail = NULL;
    inputstream->gather_func = gather_func;

    return inputstream;
}

void inputstream_free(InputStream *inputstream) {
    streamblock_free(inputstream->head);
    free(inputstream);
}

char streamblock_read_char(InputStreamBlock **head, InputStreamBlock **tail) {
    if (*head == NULL) {
        return '\0';
    }

    char result = (*head)->blockData[(*head)->used];
    (*head)->used++;

    if ((*head)->used == (*head)->occupied) {
        InputStreamBlock *next = (*head)->next;
        free((*head)->blockData);
        free(*head);
        *head = next;
        if (*head == NULL) {
            *tail = NULL;
        }
    }

    return result;
}

char inputstream_read(InputStream *inputstream, EOFMode mode) {
    char c = streamblock_read_char(&inputstream->head, &inputstream->tail);
    if (c != '\0') {
        return c;
    }
    if (streamblock_read(&inputstream->head,
                         &inputstream->tail,
                         inputstream->gather_func,
                         inputstream->data)) {
        return streamblock_read_char(&inputstream->head, &inputstream->tail);
    } else {
        switch (mode) {
        case EOF_Zero:
            return 0;
        case EOF_Negative:
            return -1;
        case EOF_Newline:
            return '\n';
        case EOF_Unchanged:
            return 0;
        }
    }
    return 0;
}

void *inputstream_get_data(InputStream *inputstream) {
    return inputstream->data;
}
