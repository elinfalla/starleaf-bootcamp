#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_VERTICES 100000
#define MAX_EDGES    1000000

static int num_vertices;
static const char *vertex_names[MAX_VERTICES];

static int num_edges;
static struct edge {
    int from;
    int to;
    int distance;
} edges[MAX_EDGES];


int dfs(int current, int to, bool *visited)
{
    if (current == to)
        return 0;

    if (visited[current])
        return -1;

    visited[current] = true;

    int best = -1;

    int i;
    for (i = 0; i < num_edges; i++) {
        int dist_after = -1;
        if (edges[i].from == current)
            dist_after = dfs(edges[i].to, to, visited);
        if (edges[i].to == current)
            dist_after = dfs(edges[i].from, to, visited);
        if (dist_after < 0)
            continue;
        if (best < 0 || best > edges[i].distance + dist_after)
            best = edges[i].distance + dist_after;
    }

    visited[current] = false;

    return best;
}

int distance(int from, int to)
{
    bool visited[MAX_VERTICES];

    int i;
    for (i = 0; i < num_vertices; i++)
        visited[i] = false;

    return dfs(from, to, visited);
}

int lookup_vertex_name(const char *name)
{
    int i;
    for (i = 0; i < num_vertices; i++) {
        if (!strcmp(name, vertex_names[i]))
            return i;
    }
    return -1;
}

int read_next_line(char *buf, size_t size, FILE *fin)
{
    while (1) {
        char *s = fgets(buf, size, fin);
        if (!s)
            return -1;
        if (buf[0] == '\0' || buf[0] == '\n' || buf[0] == '#')
            continue;

        size_t len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[--len] = '\0';
        return len;
    }
}

int main(void)
{
    FILE *fin = stdin, *fout = stdout;
    char tmp[256];
    char *token, *next;
    int from, to, dist;
    int ret, i;

    ret = read_next_line(tmp, sizeof(tmp), fin);
    if (ret < 0) {
        printf("File is empty?\n");
        return 1;
    }
    sscanf(tmp, "%d", &num_vertices);
    if (num_vertices > MAX_VERTICES) {
        printf("Too many vertices!\n");
        return 1;
    }

    for (i = 0; i < num_vertices; i++) {
        ret = read_next_line(tmp, sizeof(tmp), fin);
        if (ret < 0) {
            printf("Unexpected end of file reading vertex %d!\n", i);
            return 1;
        }

        vertex_names[i] = strdup(tmp);
        if (!vertex_names[i]) {
            printf("Out of memory!\n");
            return 1;
        }
    }

    ret = read_next_line(tmp, sizeof(tmp), fin);
    if (ret < 0) {
        printf("Unexpected end of file reading number of edges!\n");
        return 1;
    }
    sscanf(tmp, "%d", &num_edges);
    if (num_edges > MAX_VERTICES) {
        printf("Too many edges!\n");
        return 1;
    }

    for (i = 0; i < num_edges; i++) {
        if (i >= MAX_EDGES) {
            printf("Too many edges!\n");
            return 1;
        }

        ret = read_next_line(tmp, sizeof(tmp), fin);
        if (ret < 0) {
            printf("Unexpected end of file reading edge %d.\n", i);
            return 1;
        }

        token = strtok_r(tmp, " ", &next);
        from = lookup_vertex_name(token);
        if (from < 0) {
            printf("Vertex name '%s' not found!\n", token);
            return 1;
        }

        token = strtok_r(next, " ", &next);
        to = lookup_vertex_name(token);
        if (to < 0) {
            printf("Vertex name '%s' not found!\n", token);
            return 1;
        }

        token = strtok_r(next, " ", &next);
        sscanf(token, "%d", &dist);
        if (dist < 0) {
            printf("Invalid distance %d.\n", dist);
            return 1;
        }

        edges[i] = (struct edge) { from, to, dist };
    }

    for (i = 0;; i++) {
        ret = read_next_line(tmp, sizeof(tmp), fin);
        if (ret < 0) {
            // End of file.
            break;
        }

        token = strtok_r(tmp, " ", &next);
        from = lookup_vertex_name(token);
        if (from < 0) {
            printf("Vertex name '%s' not found!\n", token);
            return 1;
        }

        token = strtok_r(next, " ", &next);
        to = lookup_vertex_name(token);
        if (to < 0) {
            printf("Vertex name '%s' not found!\n", token);
            return 1;
        }

        int dist = distance(from, to);
        if (dist >= 0) {
            fprintf(fout, "%s -> %s: %d\n",
                    vertex_names[from], vertex_names[to], dist);
        } else {
            fprintf(fout, "%s and %s are not connected\n",
                    vertex_names[from], vertex_names[to]);
        }
    }

    return 0;
}
