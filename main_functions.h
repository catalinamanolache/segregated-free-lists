#ifndef MAIN_FUNCTIONS_H
#define MAIN_FUNCTIONS_H

#include "auxiliary.h"

// se creeaza nodurile din listele sfl cu adresele corespunzatoare
// incepand de la adresa de start citita de la tastatura
void fill_lists(sfl_t *heap);

// se initializeaza heap-ul
void init_heap(sfl_t *heap, int *tot_mem);

// se aloca memorie
void my_malloc(sfl_t *heap, dll_t *list, int *nr_fragm, int *mal_calls);

// se elibereaza memorie
void my_free(sfl_t *heap, dll_t *list, int *nr_fr);

// se scrie in memoria nodului cu o anumita adresa
int write(dll_t *list);

// se citeste din memoria nodului cu o anumita adresa
int read(dll_t *list);

// printarea informatiilor cerute in comanda DUMP_MEMORY
void dump_memory(sfl_t heap, dll_t list, int nr_m, int nr_f, int nr_fragm, int tot_mem);

// se elibereaza memoria alocata pentru heap
void destroy_heap(sfl_t *heap, dll_t *list);

#endif // MAIN_FUNCTIONS_H