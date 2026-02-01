#define _POSIX_C_SOURCE 200809L
#include "smart_hospital.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* helper to flush stdin leftover */
static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* create patient struct */
static Patient *create_patient(int id, const char *name, int area_id, int severity) {
    Patient *p = malloc(sizeof(Patient));
    if (!p) return NULL;
    p->id = id;
    strncpy(p->name, name ? name : "Anon", NAME_LEN-1);
    p->area_id = area_id;
    p->severity = severity;
    return p;
}

/* Process one patient: find hospital via Dijkstra and allocate bed */
static void process_patient(CityNetwork *net, Patient *p) {
    if (!net || !p) return;
    int hosp_idx = -1;
    int dist = 0;
    int res = dijkstra_shortest_to_hosp(net, p->area_id, &hosp_idx, &dist);
    if (res == 1) {
        /* found */
        int bed = hospital_allocate_bed(&net->hospitals[hosp_idx]);
        if (bed >= 0) {
            printf("\n Patient %s (id %d, severity %d) admitted to %s (bed %d) — distance %d km\n",
                   p->name, p->id, p->severity, net->hospitals[hosp_idx].name, bed, dist);
        } else {
            printf("\n Unexpected: nearest hospital reported free but allocation failed.\n");
        }
    } else if (res == 0) {
        /* none available */
        printf("\n All hospitals reachable from %s are full. Cannot admit patient %s (id %d)\n",
               net->areas[p->area_id].name, p->name, p->id);
    } else {
        printf("\n Error running routing algorithm.\n");
    }
}

/* interactive accident reporting (creates patient and processes immediately) */
static void interactive_report(CityNetwork *net, PatientPQ *pq, int *next_patient_id) {
    if (!net) return;
    printf("\nSelect accident area:\n");
    for (int i = 0; i < net->num_areas; ++i) {
        printf(" %2d - %s\n", i, net->areas[i].name);
    }
    printf("Enter area number: ");
    int aid = -1;
    if (scanf("%d", &aid) != 1) { printf("Invalid input.\n"); flush_stdin(); return; }
    if (aid < 0 || aid >= net->num_areas) { printf("Invalid area.\n"); return; }
    flush_stdin();
    char pname[NAME_LEN];
    printf("Enter patient name (or press Enter for 'Anon'): ");
    if (!fgets(pname, sizeof(pname), stdin)) { strncpy(pname, "Anon", NAME_LEN-1); }
    /* remove newline */
    char *nl = strchr(pname, '\n'); if (nl) *nl = '\0';
    if (strlen(pname) == 0) strncpy(pname, "Anon", NAME_LEN-1);
    int severity = 3;
    printf("Severity (1=critical,2=high,3=normal): ");
    if (scanf("%d", &severity) != 1) { severity = 3; }
    flush_stdin();
    Patient *p = create_patient((*next_patient_id)++, pname, aid, severity);
    if (!p) { printf("Out of memory.\n"); return; }
    /* push to PQ and immediately process queue (we simulate immediate handling) */
    if (pq_push(pq, p) != 0) { printf("Failed to queue patient.\n"); free(p); return; }
    printf("\nPatient queued. Processing queue now by priority...\n");
    while (!pq_is_empty(pq)) {
        Patient *pp = pq_pop(pq);
        if (!pp) break;
        process_patient(net, pp);
        free(pp);
    }
}

/* main menu */
int main(void) {
    CityNetwork net;
    if (!load_database(&net, "database.txt")) {
        fprintf(stderr, "Failed to load database.txt — ensure file exists in program directory.\n");
        return 1;
    }
    PatientPQ *pq = pq_create(8);
    if (!pq) { fprintf(stderr, "OOM PQ\n"); return 1; }
    int next_pid = 1001;

    int choice = -1;
    do {
        printf("\n================ SMART HOSPITAL SYSTEM ================\n");
        printf("1) Show network map (areas -> hospitals)\n");
        printf("2) Report accident (interactive)\n");
        printf("3) Show hospital bed matrix\n");
        printf("4) Show individual hospital beds (detailed)\n");
        printf("5) Save & Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { printf("Invalid input\n"); flush_stdin(); choice = -1; continue; }
        flush_stdin();
        switch (choice) {
            case 1:
                display_routes_map(&net);
                break;
            case 2:
                interactive_report(&net, pq, &next_pid);
                break;
            case 3:
                display_hospital_matrix(&net);
                break;
            case 4: {
                printf("\nChoose hospital index (0..%d):\n", net.num_hospitals-1);
                for (int i = 0; i < net.num_hospitals; ++i) printf(" %d - %s\n", i, net.hospitals[i].name);
                int hi = -1;
                if (scanf("%d", &hi) != 1) { printf("Bad input\n"); flush_stdin(); break; }
                flush_stdin();
                if (hi < 0 || hi >= net.num_hospitals) { printf("Invalid hospital\n"); break; }
                Hospital *h = &net.hospitals[hi];
                printf("\nDetails for %s\n", h->name);
                BedNode *cur = h->beds_head;
                printf("BedID | Status\n");
                while (cur) {
                    printf("%5d | %s\n", cur->bed_id, cur->occupied ? "Occupied" : "Free");
                    cur = cur->next;
                }
                break;
            }
            case 5:
                /* save and exit */
                if (!save_database(&net, "database.txt")) {
                    fprintf(stderr, "Warning: save failed\n");
                } else {
                    printf("Database saved to database.txt\n");
                }
                break;
            default:
                printf("Unknown choice\n");
        }
    } while (choice != 5);

    /* cleanup */
    pq_free(pq);
    for (int i = 0; i < net.num_hospitals; ++i) hospital_free_beds(&net.hospitals[i]);
    return 0;
}
