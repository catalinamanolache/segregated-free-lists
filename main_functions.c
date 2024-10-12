#include "main_functions.h"

void fill_lists(sfl_t *heap)
{
	unsigned int address = heap->start_address;
    // prima dimensiune (in bytes) a listelor
	unsigned int size = 8;
	unsigned int i, j;
	for (i = 0; i < heap->nr_lists; i++) {
		for (j = 0; j < heap->nr_bytes / heap->list[i]->bytes_size; j++) {
			// exista nr_bytes_per_lista / nr_bytes_lista noduri
			dll_add_nth_node(heap->list[i], j, address, size);
            // trecem la adresa urmatoare in fc de dim nodului
			address += size;
		}
		// fiecare lista are dublul dimensiunii (in bytes) celei precedente
		size *= 2;
	}
}

void init_heap(sfl_t *heap, int *tot_mem)
{
	scanf("%lx %d", &heap->start_address, &heap->nr_lists);
	scanf("%d %d", &heap->nr_bytes, &heap->recon_type);

	heap->list = malloc(heap->nr_lists * sizeof(dll_t *));
	if (!heap->list)
        DIE(1, "Failed to allocate.");

    // prima dimensiune (in bytes) a listelor
	unsigned int block_size = 8;
	for (unsigned int i = 0; i < heap->nr_lists; i++) {
        // se creeaza listele din sfl
		heap->list[i] = dll_create(block_size);

		if (!heap->list[i]) {
			for (unsigned int j = 0; j < i; j++)
				free(heap->list[j]);
			free(heap->list);
            DIE(1, "Failed to allocate.");
		}
		// fiecare lista are dublul dimensiunii (in bytes) celei precedente
		block_size *= 2;
	}
    // se completeaza listele cu noduri
	fill_lists(heap);
	// se actualizeaza memoria totala pentru comanda DUMP_MEMORY
	*tot_mem += heap->nr_lists * heap->nr_bytes;
}

void my_malloc(sfl_t *heap, dll_t *list, int *nr_fragm, int *mal_calls)
{
	dll_node_t *node;
	unsigned int nr_bytes;
	scanf("%d", &nr_bytes);

    // se cauta dimensiunea maxima disponibila pt alocare
	unsigned int max = 0;
	for (unsigned int i = 0; i < heap->nr_lists; i++)
		if (heap->list[i]->bytes_size > max && heap->list[i]->size != 0)
			max = heap->list[i]->bytes_size;

    // nu exista un block care sa cuprinda memoria ceruta
	if (nr_bytes > max) { 
		printf("Out of memory\n");
		return;
	}

	for (unsigned int i = 0; i < heap->nr_lists; i++) {
		if (heap->list[i]->bytes_size == nr_bytes && heap->list[i]->size != 0) {
			// se gaseste o lista cu dimensiunea egala cu memoria ceruta
			node = dll_remove_nth_node(heap->list[i], 0); // adresa minima
			unsigned int n = add_ascending(list, node->address);
			dll_add_nth_node(list, n, node->address, node->size);
			// parametri pentru DUMP_MEMORY

            // creste nr de bytes alocati
			list->bytes_size += nr_bytes;
            // creste numarul de apeluri ale fc malloc
			(*mal_calls)++;
			free(node);
			return;
		} else if (heap->list[i]->bytes_size > nr_bytes &&
					heap->list[i]->size != 0) {
			// nu exista o lista cu dim corespunzatoare memoriei cerute
            // creste numarul de fragmentari
			(*nr_fragm)++;
			node = dll_remove_nth_node(heap->list[i], 0);
            // dim nodului liber
			unsigned int new_size = node->size - nr_bytes;
            // adresa nodului liber
			unsigned int new_address = node->address + nr_bytes;
            // se actualizeaza dimensiunea nodului alocat
			node->size = nr_bytes;
			// cauta daca exista o lista in sfl cu dimensiunea nodului liber
			unsigned int j = 0;
			for (j = 0; j < heap->nr_lists; j++)
                // verificam daca listele nu sunt goale
				if (heap->list[j]->bytes_size == new_size &&
					heap->list[j]->size != 0) 
					break;

			if (j == heap->nr_lists) {
				// nu s-a gasit, se creeaza lista cu dimensiunea nodului liber
                // se realoca sfl
				heap->nr_lists++;
				size_t realloc_size = heap->nr_lists * sizeof(dll_t *);
				heap->list = realloc(heap->list, realloc_size);
                DIE(!heap->list, "realloc failed");

				int k = 0;
                // se cauta pozitia unde va fi inserata noua lista
				while (heap->list[k]->bytes_size < new_size)
					k++;

				// se creeaza spatiu pt noua lista mutand listele
				for (int l = heap->nr_lists - 1; l > k; l--)
					heap->list[l] = heap->list[l - 1];
				heap->list[k] = dll_create(new_size);
				if (!heap->list[k])
					DIE(1, "Failed to allocate.");

				// se adauga nodul liber in aceasta lista
				dll_add_nth_node(heap->list[k], heap->list[k]->size,
								 new_address, new_size);
			} else {
				// s-a gasit o lista cu dim egala, se adauga nodul crescator
				unsigned int n = add_ascending(heap->list[j], new_address);
				dll_add_nth_node(heap->list[j], n, new_address, new_size);
			}
			// se adauga nodul in lista cu memorie alocata
			unsigned int n = add_ascending(list, node->address);
			dll_add_nth_node(list, n, node->address, node->size);
			// parametri pentru DUMP_MEMORY
            // creste nr de bytes alocati
			list->bytes_size += nr_bytes;
            // creste numarul de apeluri ale fc malloc
			(*mal_calls)++;
			free(node);
			return;
		}
	}
}

void my_free(sfl_t *heap, dll_t *list, int *nr_fr)
{
	unsigned int address;
	scanf("%x", &address);
	if (address == 0)
		return;
	// se cauta pozitia nodului cu adresa data in lista cu memorie alocata
	int node_pos = find_node(list, address);
	if (node_pos == -1) {
		// nu exista un nod alocat cu aceasta adresa
		printf("Invalid free\n");
		return;
	}
    // se scoate nodul din lista
	dll_node_t *node = dll_remove_nth_node(list, node_pos);
	unsigned int i;
	for (i = 0; i < heap->nr_lists; i++)
		if (heap->list[i]->bytes_size == node->size) {
			// se gaseste o lista cu dimensiunea egala cu cea a nodului
			unsigned int n = add_ascending(heap->list[i], address);
			dll_add_nth_node(heap->list[i], n, node->address, node->size);
			// se actualizeaza numarul de bytes alocati
			list->bytes_size -= node->size;
            // se elibereaza memoria nodului
			if (node->data)
				free(node->data);
			free(node);
            // se creste numarul de free-uri
			(*nr_fr)++;
			return;
		}

	if (i == heap->nr_lists) {
	// nu s-a gasit, se creeaza lista cu dimensiunea nodului liber
        // se realoca sfl
		heap->nr_lists++;
		heap->list = realloc(heap->list, heap->nr_lists * sizeof(dll_t *));
		DIE(!heap->list, "realloc failed");

		int j = 0;
        // se cauta pozitia unde va fi inserata noua lista
		while (heap->list[j]->bytes_size < node->size)
			j++;
		// se creeaza spatiu pt noua lista mutand listele
		for (int k = heap->nr_lists - 1; k > j; k--)
			heap->list[k] = heap->list[k - 1];

		heap->list[j] = dll_create(node->size);
		if (!heap->list[j])
            DIE(1, "Failed to allocate.");
	
		// adaug nodul crescator la inceput
		dll_add_nth_node(heap->list[j], heap->list[j]->size,
						 node->address, node->size);
		// se actualizeaza numarul de bytes alocati
		list->bytes_size -= node->size;
        // se elibereaza memoria nodului
		if (node->data)
			free(node->data);
		free(node);
        // se creste numarul de free-uri
		(*nr_fr)++;
		return;
	}
}

int write(dll_t *list)
{
	unsigned int nr_bytes;
	size_t address;
	char line[600], data[100];

	scanf("%lx", &address);
	fgets(line, sizeof(line), stdin);
	// prelucrarea liniei
	char delim[] = "\"";
	char *p = strtok(line, delim);
	p = strtok(NULL, delim);
	strcpy(data, p);
	p = strtok(NULL, delim);
	nr_bytes = atoi(p);

    // se cauta pozitia nodului cu adresa data in lista cu memorie alocata
	dll_node_t *aux = list->head;
	int n = find_node(list, address);
	if (n == -1) {
		// nu exista un nod alocat cu aceasta adresa
		printf("Segmentation fault (core dumped)\n");
		return -1;
	}
	// se cauta nodul
	for (int i = 0; i < n; i++)
		aux = aux->next;
	// daca nr de bytes de scris este mai mare decat lungimea stringului
    // se scriu doar strlen(string) bytes
	if (nr_bytes > strlen(data))
		nr_bytes = strlen(data);

	if (nr_bytes <= aux->size) {
		// daca nr de bytes de scris incap intr-un singur nod
		if (aux->data) {
			char s[200];
			// daca nodul avea deja	informatii in el, se suprascrie o portiune
			// daca portiunea este mai mica decat lungimea stringului original
			if (strlen(data) < strlen((char *)aux->data)) {
                // se suprascrie doar o portiune
				strcpy(s, data);
				strcat(s, (char *)aux->data + strlen(data));
			} else {
                // se suprascrie total stringul
				strcpy(s, data);
			}
			// se revine la variabila originala
			strcpy(data, s);
			// se realoca memorie pt noua informatie si se copiaza
			aux->data = realloc(aux->data, strlen(data) + 1);
            DIE(!aux->data, "realloc failed");
    
			memcpy(aux->data, data, strlen(data) + 1);
			((char *)aux->data)[strlen(data)] = '\0';
			return 0;
		}
		// daca nodul era gol
		// se realoca memorie pt noua informatie si se copiaza
		aux->data = realloc(aux->data, nr_bytes + 1);
        DIE(!aux->data, "realloc failed");

		memcpy(aux->data, data, nr_bytes + 1);
		((char *)aux->data)[nr_bytes] = '\0';
		return 0;
	}
	unsigned int bytes_written = 0;
	while (aux && bytes_written != nr_bytes) {
	// daca nr de bytes de scris nu incap intr-un singur nod
	// se trece la urmatoarele noduri pana s-au scris nr_bytes octeti

    // daca lista nu s-a terminat
		if (aux->next)
			if (aux->address + aux->size != aux->next->address) {
				// daca adresele la care se scrie nu sunt continue
				printf("Segmentation fault (core dumped)\n");
				return -1;
			}
		// se realoca memorie pt noua informatie si se copiaza
		aux->data = realloc(aux->data, aux->size + 1);
        DIE(!aux->data, "realloc failed");

		memcpy(aux->data, data + bytes_written, aux->size + 1);
		((char *)aux->data)[aux->size] = '\0';
		bytes_written += aux->size;
		aux = aux->next;
	}
	return 0;
}

int read(dll_t *list)
{
	unsigned int nr_bytes;
	size_t address;
	scanf("%lx %d", &address, &nr_bytes);

    // se cauta pozitia nodului cu adresa data in lista cu memorie alocata
	dll_node_t *aux = list->head;
	int n = find_node(list, address);
	if (n == -1) {
		// nu exista un nod alocat cu aceasta adresa
		printf("Segmentation fault (core dumped)\n");
		return -1;
	}
	// se cauta nodul
	for (int i = 0; i < n; i++)
		aux = aux->next;

	if (nr_bytes <= aux->size) {
		// daca nr de bytes de citit incap intr-un singur nod
		char new_data[200];
		// se copiaza nr_bytes octeti din informatiile nodului pt a fi cititi
		strncpy(new_data, (char *)aux->data, nr_bytes);
		new_data[nr_bytes] = '\0';
		printf("%s\n", new_data);
		return 0;
	}
	unsigned int bytes_read = 0;
	while (aux && bytes_read != nr_bytes) {
	// daca nr de bytes de citit nu incap intr-un singur nod
	// se trece la urmatoarele noduri pana s-au citit nr_bytes octeti
		if (aux->next) // daca lista nu s-a terminat
			if (aux->address + aux->size != aux->next->address) {
				// daca adresele de unde se citeste nu sunt continue
				printf("Segmentation fault (core dumped)\n");
				return -1;
			}
		printf("%s", (char *)aux->data);
		bytes_read += aux->size;
		aux = aux->next;
	}
	printf("\n");
	return 0;
}

void dump_memory(sfl_t heap, dll_t list, int nr_m,
				 int nr_f, int nr_fragm, int tot_mem)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", tot_mem);
	printf("Total allocated memory: %d bytes\n", list.bytes_size);
	printf("Total free memory: %d bytes\n", tot_mem - list.bytes_size);
	int free_blocks = 0;
	// se calculeaza nr de noduri libere in sfl
	for (unsigned int i = 0; i < heap.nr_lists; i++)
		if (heap.list[i]->size)
			free_blocks += heap.list[i]->size;
	printf("Free blocks: %d\n", free_blocks);
	printf("Number of allocated blocks: %d\n", list.size);
	printf("Number of malloc calls: %d\n", nr_m);
	printf("Number of fragmentations: %d\n", nr_fragm);
	printf("Number of free calls: %d\n", nr_f);
	for (unsigned int i = 0; i < heap.nr_lists; i++) {
        // daca lista e goala se trece peste ea
		if (heap.list[i]->size == 0)
			continue;
		// printare blockuri sfl
		printf("Blocks with %d bytes - %d free block(s) :",
			   heap.list[i]->bytes_size, heap.list[i]->size);
		dll_node_t *aux = heap.list[i]->head;
		for (unsigned int j = 0; j < heap.list[i]->size; j++) {
            // nodurile din fiecare lista sfl
			printf(" 0x%lx", aux->address);
			aux = aux->next;
		}
		printf("\n");
	}
	printf("Allocated blocks :");
	if (list.head) {
		dll_node_t *aux = list.head;
	// printare blockuri lista cu memoria alocata
		while (aux) {
			printf(" (0x%lx - %d)", aux->address, aux->size);
			aux = aux->next;
		}
	}
	printf("\n-----DUMP-----\n");
}

void destroy_heap(sfl_t *heap, dll_t *list)
{ // se elibereaza sfl si lista de memorie alocata
	for (unsigned int i = 0; i < heap->nr_lists; i++)
		if (heap->list[i])
			free_list(&heap->list[i]);
	free(heap->list);
	free_list(&list);
}