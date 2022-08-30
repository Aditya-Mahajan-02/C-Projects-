#include "path.h"

#include "stack.h"
#include "vertices.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Path {

    Stack *vertices;
    uint32_t length;
};

Path *path_create(void) {

    Path *p = (Path *) malloc(sizeof(Path));
    if (p) {
        p->length = 0;
        p->vertices = stack_create(VERTICES);
        if (!p->vertices) {
            free(p);
            p = NULL;
        }
    }
    return p;
}

void path_delete(Path **p) {
    free(*p);
    *p = NULL;
    return;
}

bool path_push_vertex(Path *p, uint32_t v, Graph *G) {

    uint32_t top = 0;
    stack_peek(p->vertices, &top);
    if (stack_full(p->vertices)) {
        return false;
    } else {
        stack_push(p->vertices, v);
        p->length += graph_edge_weight(G, top, v);
        return true;
    }
}
bool path_pop_vertex(Path *p, uint32_t *v, Graph *G) {
    if (stack_empty(p->vertices)) {
        return false;
    } else {
        stack_pop(p->vertices, v);
        uint32_t top = 0;
        stack_peek(p->vertices, &top);
        p->length -= graph_edge_weight(G, top, *v);
        return true;
    }
}
uint32_t path_vertices(Path *p) {

    return stack_size(p->vertices);
}

uint32_t path_length(Path *p) {

    return p->length;
}
void path_copy(Path *dst, Path *src) {
    assert(dst);
    stack_copy(dst->vertices, src->vertices);
    dst->length = src->length;
}

void path_print(Path *p, FILE *outfile, char *cities[]) {

    fprintf(outfile, "Path length: %u\n", p->length);
    fprintf(outfile, "Path: ");
    stack_print(p->vertices, outfile, cities);
}
