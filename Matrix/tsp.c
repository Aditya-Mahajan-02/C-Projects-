
#include "graph.h"
#include "path.h"
#include "stack.h"
#include "vertices.h"
#define OPTIONS "hvui:o:"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static uint32_t recursive_calls = 0;
void dfs(Graph *G, uint32_t v, Path *curr, Path *shortest, char *cities[], FILE *outfile,
    bool print_verbose) {

    graph_mark_visited(G, v);
    recursive_calls += 1;
    if (graph_vertices(G) == path_vertices(curr)) {
        if (graph_has_edge(G, v, START_VERTEX)) {
            path_push_vertex(curr, START_VERTEX, G);
            if (path_length(curr) < path_length(shortest) || path_length(shortest) == 0) {
                path_copy(shortest, curr);
            }
            uint32_t placer;
            path_pop_vertex(curr, &placer, G);
        }
    }
    for (uint32_t w = 0; w < graph_vertices(G); w++) {
        if (graph_has_edge(G, v, w) && graph_visited(G, w) == false) {
            uint32_t vertex;
            path_push_vertex(curr, w, G);
            if (print_verbose) {
                path_print(curr, outfile, cities);
            }
            dfs(G, w, curr, shortest, cities, outfile, print_verbose);
            path_pop_vertex(curr, &vertex, G);
        }
    }
    graph_mark_unvisited(G, v);
}

int main(int argc, char **argv) {
    bool case_h = false;
    bool case_v = false;
    bool case_u = false;

    FILE *infile = stdin;
    FILE *outfile = stdout;
    uint32_t cities = 0;
    fscanf(infile, "%d\n", &cities);
    if (cities > 26) {

        printf("Invalid input too many vertices.");
        scanf("%d", &cities);
    }
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': case_h = true; break;
        case 'v': case_v = true; break;
        case 'u': case_u = true; break;
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        default: case_h = true; break;
        }
    }
    if (case_h) {
        printf("SYNOPSIS\n   Traveling Salesman Problem using DFS.\n\nUSAGE\n  ./tsp [-u] [-v] "
               "[-h] [-i infile] [-o outfile]\n\nOPTIONS\n  -u             Use undirected graph.\n "
               " -v             Enable verbose printing.\n  -h             Program usage and "
               "help.\n  -i infile      Input containing graph (default: stdin)\n  -o outfile     "
               "Output of computed path (default: stdout)\n");
    } else {

        char **city_list = (char **) malloc(cities * sizeof(char *));
        char city_names[1024];
        for (int i = 0; i <= cities; i++) {
            fgets(city_names, 1024, infile);
            city_names[strlen(city_names) - 1] = '\0';
            city_list[i] = strdup(city_names);
        }
        Graph *G = graph_create(cities, case_u);
        if (G == NULL) {
            fscanf(stderr, "Error creating graph.");
            graph_delete(&G);
        }
        uint32_t i_vert = 0;
        uint32_t j_vert = 0;
        uint32_t k_side = 0;

        while (NULL != fgets(city_names, 1024, infile)) {
            sscanf(city_names, "%u %u %u", &i_vert, &j_vert, &k_side);
            graph_add_edge(G, i_vert, j_vert, k_side);
        }
        Path *current_path = path_create();
        path_push_vertex(current_path, START_VERTEX, G);
        Path *short_path = path_create();

        dfs(G, START_VERTEX, current_path, short_path, city_list, outfile, case_v);

        path_print(short_path, outfile, city_list);
        printf("Total recursive calls: %d\n", recursive_calls);

        path_delete(&short_path);
        path_delete(&current_path);
        graph_delete(&G);
    }
}
