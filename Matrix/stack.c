#include "stack.h"

#include "vertices.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Stack {

    uint32_t top;
    uint32_t capacity;
    uint32_t *items;
};

Stack *stack_create(uint32_t capacity) {

    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (uint32_t *) calloc(capacity, sizeof(uint32_t));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}
void stack_delete(Stack **s) {
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

bool stack_empty(Stack *s) {
    if (s->top == 0) {
        return true;
    }
    return false;
    //return (s->top == 0);
}

bool stack_full(Stack *s) {
    //  return (s->top == s->capacity);
    if (s->top == s->capacity) {
        return true;
    } else {
        return false;
    }
}

uint32_t stack_size(Stack *s) {
    return (s->top);
}
bool stack_push(Stack *s, uint32_t x) {

    if (s->top == s->capacity) {
        return false;
    } else {
        s->items[s->top] = x;
        s->top += 1;

        return true;
    }
}
bool stack_pop(Stack *s, uint32_t *x) {

    if (s->top == 0) {

        return false;
    } else {
        *x = s->items[s->top - 1];
        s->top -= 1;
        return true;
    }
}

bool stack_peek(Stack *s, uint32_t *x) {
    if (!stack_empty(s)) {
        *x = s->items[s->top - 1];
        return *x;
    } else {

        return false;
    }
}

void stack_copy(Stack *dst, Stack *src) {
    assert(dst);
    assert(src);
    if (dst->capacity >= src->top) {
        for (uint32_t count = 0; count < src->capacity; count += 1) {
            dst->items[count] = src->items[count];
        }
        dst->top = src->top;
    }
}

void stack_print(Stack *s, FILE *outfile, char *cities[]) {
    for (uint32_t i = 0; i < s->top; i += 1) {
        fprintf(outfile, "%s", cities[s->items[i]]);
        if (i + 1 != s->top) {
            fprintf(outfile, " -> ");
        }
    }
    fprintf(outfile, "\n");
}
