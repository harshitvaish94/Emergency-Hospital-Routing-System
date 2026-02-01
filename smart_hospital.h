#ifndef SMART_HOSPITAL_H
#define SMART_HOSPITAL_H

#include <stddef.h>

#define MAX_HOSPITALS 30
#define MAX_AREAS 50
#define MAX_NODES (MAX_AREAS + MAX_HOSPITALS)
#define NAME_LEN 64
#define INF_DIST 1000000000

typedef struct BedNode {
    int bed_id;
    int occupied;
    struct BedNode *next;
} BedNode;

typedef struct Hospital {
    int id;
    char name[NAME_LEN];
    int total_beds;
    BedNode *beds_head;
} Hospital;

typedef struct Area {
    int id;
    char name[NAME_LEN];
} Area;

typedef struct CityNetwork {
    Hospital *hospitals;
    Area *areas;
    int num_hospitals;
    int num_areas;
    int **adj;
} CityNetwork;

typedef struct Patient {
    int id;
    char name[NAME_LEN];
    int area_id;
    int severity;
} Patient;

typedef struct PatientPQ {
    Patient **arr;
    size_t size;
    size_t cap;
} PatientPQ;

/* Database functions */
int load_database(CityNetwork *net, const char *filename);
int save_database(const CityNetwork *net, const char *filename);

/* Hospital helpers */
int hospital_init_beds(Hospital *h, int total_beds);
void hospital_free_beds(Hospital *h);
int hospital_allocate_bed(Hospital *h);
int hospital_free_bed_count(const Hospital *h);

/* Graph & routing */
int dijkstra_shortest_to_hosp(const CityNetwork *net, int area_idx, int *out_hosp_idx, int *out_dist);

/* Priority queue */
PatientPQ *pq_create(size_t cap);
void pq_free(PatientPQ *pq);
int pq_push(PatientPQ *pq, Patient *p);
Patient *pq_pop(PatientPQ *pq);
int pq_is_empty(const PatientPQ *pq);

/* UI helpers */
void display_routes_map(const CityNetwork *net);
void display_hospital_matrix(const CityNetwork *net);
void print_hospital_status(const Hospital *h);

/* Cleanup */
void free_city_network(CityNetwork *net);

#endif
