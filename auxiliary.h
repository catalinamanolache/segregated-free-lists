#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DIE(assertion, call_description) \
    do { \
        if (assertion) { \
            fprintf(stderr, "(%s, %d): ", \
                    __FILE__, __LINE__); \
            perror(call_description); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

typedef struct dll_node_t dll_node_t;
typedef struct dll_t dll_t;
typedef struct sfl_t sfl_t;

struct dll_node_t {
	dll_node_t *prev, *next;
	size_t address;
	void *data;
    // dimensiunea in bytes
	unsigned int size;
};

struct dll_t {
	dll_node_t *head;
    // dimensiunea tuturor nodurilor din aceasta lista
	unsigned int bytes_size;
    // dimensiunea in noduri
	unsigned int size;
};

struct sfl_t {
    // dimensiunea vectorului de liste
	unsigned int nr_lists;
    // numarul de bytes per lista
	unsigned int nr_bytes;
    // tipul reconstituirii
	unsigned int recon_type;
    // adresa de start a heap-ului
	unsigned long start_address;
	dll_t **list;
};

// se adauga un nod la pozitia n dintr-o lista dublu inlantuita
void dll_add_nth_node(dll_t *list, unsigned int n,
                      size_t address, unsigned int size);

// se elimina nodul de pe pozitia n dintr-o lista dublu inlantuita
dll_node_t *dll_remove_nth_node(dll_t *list, unsigned int n);

// se adauga un nod intr-o lista dublu inlantuita in ordinea 
// crescatoare a adresei
unsigned int add_ascending(dll_t *list, unsigned int address);

// se cauta nodul cu adresa address intr-o lista dublu inlantuita
int find_node(dll_t *list, unsigned int address);

// se creeaza si se initializeaza o lista dublu inlantuita
dll_t *dll_create(unsigned int size);

// se elibereaza memoria alocata pentru o lista dublu inlantuita
void free_list(dll_t **list);

#endif  // AUXILIARY_H
