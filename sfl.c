// Copyright Manolache Maria-Catalina 313CAb 2023-2024
#include "auxiliary.h"
#include "main_functions.h"

int main(void)
{
	char command[50];
	sfl_t heap;
	dll_t *list = dll_create(0);

	// parametri pentru DUMP_MEMORY
	int nr_malloc_calls = 0, nr_fragm = 0, nr_free_calls = 0, tot_mem = 0;
	while (1) {
		scanf("%s", command);
		if (strcmp(command, "INIT_HEAP") == 0) {
			init_heap(&heap, &tot_mem);
		} else if (strcmp(command, "MALLOC") == 0) {
			my_malloc(&heap, list, &nr_fragm, &nr_malloc_calls);
		} else if (strcmp(command, "FREE") == 0) {
			my_free(&heap, list, &nr_free_calls);
		} else if (strcmp(command, "READ") == 0) {
			// daca are loc segmentation fault, se face un DUMP MEMORY
			// si se elibereaza memoria alocata pentru heap
			if (read(list) == -1) {
				dump_memory(heap, *list, nr_malloc_calls,
							nr_free_calls, nr_fragm, tot_mem);
				destroy_heap(&heap, list);
				return 0;
			}
		} else if (strcmp(command, "WRITE") == 0) {
			// daca are loc segmentation fault, se face un DUMP MEMORY
			// si se elibereaza memoria alocata pentru heap
			if (write(list) == -1) {
				dump_memory(heap, *list, nr_malloc_calls,
							nr_free_calls, nr_fragm, tot_mem);
				destroy_heap(&heap, list);
				return 0;
			}
		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump_memory(heap, *list, nr_malloc_calls,
						nr_free_calls, nr_fragm, tot_mem);
		} else if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy_heap(&heap, list);
			break;
		}
	}
	return 0;
}
