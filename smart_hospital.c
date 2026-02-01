#define _POSIX_C_SOURCE 200809L
#include "smart_hospital.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* ---------------- Utility ---------------- */
static void *safe_malloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* ---------------- Database Loader ---------------- */
int load_database(CityNetwork *net, const char *filename) {
    if (!net || !filename) return 0;
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("open database");
        return 0;
    }
    memset(net, 0, sizeof(*net));

    char token[256];
    fscanf(f, "%255s", token);
    if (strcmp(token, "HOSPITALS") != 0) { fclose(f); return 0; }
    fscanf(f, "%d", &net->num_hospitals);
    net->hospitals = safe_malloc(sizeof(Hospital) * net->num_hospitals);

    for (int i = 0; i < net->num_hospitals; i++) {
        char name[NAME_LEN];
        int total, occ;
        fscanf(f, "%63s %d %d", name, &total, &occ);
        for (char *p = name; *p; p++) if (*p == '_') *p = ' ';
        strncpy(net->hospitals[i].name, name, NAME_LEN - 1);
        net->hospitals[i].id = i;
        hospital_init_beds(&net->hospitals[i], total);
        BedNode *cur = net->hospitals[i].beds_head;
        while (occ-- > 0 && cur) {
            cur->occupied = 1;
            cur = cur->next;
        }
    }

    fscanf(f, "%255s", token);
    if (strcmp(token, "AREAS") != 0) { fclose(f); return 0; }
    fscanf(f, "%d", &net->num_areas);
    net->areas = safe_malloc(sizeof(Area) * net->num_areas);
    for (int i = 0; i < net->num_areas; i++) {
        char name[NAME_LEN];
        fscanf(f, "%63s", name);
        for (char *p = name; *p; p++) if (*p == '_') *p = ' ';
        strncpy(net->areas[i].name, name, NAME_LEN - 1);
        net->areas[i].id = i;
    }

    fscanf(f, "%255s", token);
    if (strcmp(token, "DISTANCES") != 0) { fclose(f); return 0; }

    int N = net->num_areas + net->num_hospitals;
    net->adj = safe_malloc(sizeof(int*) * N);
    for (int i = 0; i < N; i++) {
        net->adj[i] = safe_malloc(sizeof(int) * N);
        for (int j = 0; j < N; j++) net->adj[i][j] = INF_DIST;
    }

    for (int i = 0; i < net->num_areas; i++) {
        for (int j = 0; j < net->num_hospitals; j++) {
            int d;
            if (fscanf(f, "%d", &d) == 1) {
                int area_node = i;
                int hosp_node = net->num_areas + j;
                net->adj[area_node][hosp_node] = d;
                net->adj[hosp_node][area_node] = d;
            }
        }
    }

    fclose(f);
    return 1;
}
/* ---------------- Save Database ---------------- */
int save_database(const CityNetwork *net, const char *filename) {
    if (!net || !filename) return 0;
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("save database");
        return 0;
    }

    fprintf(f, "HOSPITALS %d\n", net->num_hospitals);
    for (int i = 0; i < net->num_hospitals; i++) {
        char namebuf[NAME_LEN];
        strncpy(namebuf, net->hospitals[i].name, NAME_LEN - 1);
        for (char *p = namebuf; *p; ++p) if (*p == ' ') *p = '_';

        int occupied = 0;
        BedNode *cur = net->hospitals[i].beds_head;
        while (cur) {
            if (cur->occupied) occupied++;
            cur = cur->next;
        }
        fprintf(f, "%s %d %d\n", namebuf,
                net->hospitals[i].total_beds, occupied);
    }

    fprintf(f, "AREAS %d\n", net->num_areas);
    for (int i = 0; i < net->num_areas; i++) {
        char namebuf[NAME_LEN];
        strncpy(namebuf, net->areas[i].name, NAME_LEN - 1);
        for (char *p = namebuf; *p; ++p) if (*p == ' ') *p = '_';
        fprintf(f, "%s\n", namebuf);
    }

    fprintf(f, "DISTANCES\n");
    for (int i = 0; i < net->num_areas; i++) {
        for (int j = 0; j < net->num_hospitals; j++) {
            int area_node = i;
            int hosp_node = net->num_areas + j;
            int d = net->adj[area_node][hosp_node];
            if (d >= INF_DIST) d = 0;
            fprintf(f, "%d ", d);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}


/* ---------------- Bed Management ---------------- */
int hospital_init_beds(Hospital *h, int total) {
    h->beds_head = NULL;
    h->total_beds = total;
    for (int i = total; i >= 1; i--) {
        BedNode *b = safe_malloc(sizeof(BedNode));
        b->bed_id = i;
        b->occupied = 0;
        b->next = h->beds_head;
        h->beds_head = b;
    }
    return 0;
}

void hospital_free_beds(Hospital *h) {
    BedNode *cur = h->beds_head;
    while (cur) {
        BedNode *next = cur->next;
        free(cur);
        cur = next;
    }
}

int hospital_allocate_bed(Hospital *h) {
    for (BedNode *cur = h->beds_head; cur; cur = cur->next) {
        if (!cur->occupied) {
            cur->occupied = 1;
            return cur->bed_id;
        }
    }
    return -1;
}

int hospital_free_bed_count(const Hospital *h) {
    int c = 0;
    for (BedNode *cur = h->beds_head; cur; cur = cur->next)
        if (!cur->occupied) c++;
    return c;
}

/* ---------------- Graph / Dijkstra ---------------- */
int dijkstra_shortest_to_hosp(const CityNetwork *net, int area_idx, int *out_hosp, int *out_dist) {
    int N = net->num_areas + net->num_hospitals;
    int *dist = safe_malloc(sizeof(int) * N);
    int *visited = calloc(N, sizeof(int));

    for (int i = 0; i < N; i++) dist[i] = INF_DIST;
    dist[area_idx] = 0;

    for (int count = 0; count < N; count++) {
        int u = -1, mind = INF_DIST;
        for (int i = 0; i < N; i++)
            if (!visited[i] && dist[i] < mind) { mind = dist[i]; u = i; }

        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < N; v++) {
            int w = net->adj[u][v];
            if (w < INF_DIST && dist[u] + w < dist[v])
                dist[v] = dist[u] + w;
        }
    }

    int best = -1, best_dist = INF_DIST;
    for (int h = 0; h < net->num_hospitals; h++) {
        int node = net->num_areas + h;
        if (hospital_free_bed_count(&net->hospitals[h]) > 0 && dist[node] < best_dist) {
            best_dist = dist[node];
            best = h;
        }
    }

    free(dist); free(visited);
    if (best == -1) return 0;
    *out_hosp = best;
    *out_dist = best_dist;
    return 1;
}

/* ---------------- Cleanup ---------------- */
void free_city_network(CityNetwork *net) {
    for (int i = 0; i < net->num_hospitals; i++)
        hospital_free_beds(&net->hospitals[i]);
    for (int i = 0; i < net->num_areas + net->num_hospitals; i++)
        free(net->adj[i]);
    free(net->adj);
    free(net->areas);
    free(net->hospitals);
}

/* ---------------- Priority Queue ---------------- */
static int patient_cmp(const Patient *a, const Patient *b) {
    if (a->severity != b->severity) return (a->severity < b->severity) ? -1 : 1;
    return (a->id < b->id) ? -1 : 1;
}

static void pq_swap(Patient **a, Patient **b) {
    Patient *t = *a;
    *a = *b;
    *b = t;
}

PatientPQ *pq_create(size_t cap) {
    if (cap == 0) cap = 8;
    PatientPQ *pq = malloc(sizeof(PatientPQ));
    if (!pq) return NULL;
    pq->arr = malloc(sizeof(Patient*) * cap);
    pq->size = 0;
    pq->cap = cap;
    return pq;
}

void pq_free(PatientPQ *pq) {
    if (!pq) return;
    for (size_t i = 0; i < pq->size; i++)
        free(pq->arr[i]);
    free(pq->arr);
    free(pq);
}

int pq_push(PatientPQ *pq, Patient *p) {
    if (!pq || !p) return -1;
    if (pq->size >= pq->cap) {
        pq->cap *= 2;
        pq->arr = realloc(pq->arr, sizeof(Patient*) * pq->cap);
    }
    size_t i = pq->size++;
    pq->arr[i] = p;
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (patient_cmp(pq->arr[parent], pq->arr[i]) <= 0) break;
        pq_swap(&pq->arr[parent], &pq->arr[i]);
        i = parent;
    }
    return 0;
}

Patient *pq_pop(PatientPQ *pq) {
    if (!pq || pq->size == 0) return NULL;
    Patient *res = pq->arr[0];
    pq->arr[0] = pq->arr[--pq->size];
    size_t i = 0;
    while (1) {
        size_t left = 2*i + 1, right = 2*i + 2, smallest = i;
        if (left < pq->size && patient_cmp(pq->arr[left], pq->arr[smallest]) < 0)
            smallest = left;
        if (right < pq->size && patient_cmp(pq->arr[right], pq->arr[smallest]) < 0)
            smallest = right;
        if (smallest == i) break;
        pq_swap(&pq->arr[i], &pq->arr[smallest]);
        i = smallest;
    }
    return res;
}

int pq_is_empty(const PatientPQ *pq) {
    return (!pq || pq->size == 0);
}

/* ---------------- Display Helpers ---------------- */
void display_routes_map(const CityNetwork *net) {
    if (!net) return;
    printf("\n=============== CITY ROUTE MAP ===============\n");
    for (int i = 0; i < net->num_areas; i++) {
        printf("%-18s → ", net->areas[i].name);
        for (int j = 0; j < net->num_hospitals; j++) {
            int dist = net->adj[i][net->num_areas + j];
            printf("%s (%dkm)", net->hospitals[j].name, dist);
            if (j < net->num_hospitals - 1) printf(" | ");
        }
        printf("\n");
    }
    printf("==============================================\n");
}

void display_hospital_matrix(const CityNetwork *net) {
    if (!net) return;
    printf("\n🏥  HOSPITAL BED MATRIX\n");
    printf("------------------------------------------------------------\n");
    printf("%-35s | %-10s | %-10s\n", "Hospital", "Total Beds", "Free Beds");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < net->num_hospitals; i++) {
        int freec = hospital_free_bed_count(&net->hospitals[i]);
        printf("%-35s | %-10d | %-10d\n", net->hospitals[i].name,
               net->hospitals[i].total_beds, freec);
    }
    printf("------------------------------------------------------------\n");
}
