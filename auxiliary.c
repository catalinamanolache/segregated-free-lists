#include "auxiliary.h"

void dll_add_nth_node(dll_t *list, unsigned int n,
					  size_t address, unsigned int size)
{
	dll_node_t *new_node = malloc(1 * sizeof(dll_node_t));
    DIE(!new_node, "malloc");

	new_node->address = address;
	new_node->size = size;
	new_node->data = NULL;

	if (!list->head) {
		// daca lista e goala
		list->head = new_node;
		new_node->next = NULL;
		new_node->prev = NULL;
		list->size++;
	} else if (n == 0) {
		// adaugare pe prima pozitie
		new_node->next = list->head;
		new_node->prev = NULL;
		list->head->prev = new_node;
		list->head = new_node;
		list->size++;
	} else if (n >= list->size) {
		// adaugare la finalul listei
		dll_node_t *aux = list->head;
		while (aux->next)
			aux = aux->next;

		aux->next = new_node;
		new_node->next = NULL;
		new_node->prev = aux;
		list->size++;
	} else {
		// adaugare generica, pe orice pozitie
		dll_node_t *aux = list->head;
		for (unsigned int i = 0; i < n - 1; i++)
			aux = aux->next;
		new_node->next = aux->next;
		aux->next->prev = new_node;
		aux->next = new_node;
		new_node->prev = aux;
		list->size++;
	}
}

dll_node_t *dll_remove_nth_node(dll_t *list, unsigned int n)
{
	if (!list->head) {
		// daca lista e goala se returneaza tipul eroare
		dll_node_t *error = malloc(1 * sizeof(dll_node_t));
		return error;
	} else if (n == 0) {
		// eliminare de pe prima pozitie
		dll_node_t *torem = list->head;
		list->head = torem->next;
		list->size--;
		return torem;
	} else if (n >= list->size) {
		// eliminare de la finalul listei
		dll_node_t *aux = list->head;
		while (aux->next)
			aux = aux->next;
		dll_node_t *torem = aux;
		torem->prev->next = NULL;
		list->size--;
		return torem;
	}
		// eliminare generica, de pe orice pozitie
		dll_node_t *aux = list->head;
		for (unsigned int i = 0; i < n - 1; i++)
			aux = aux->next;
		dll_node_t *torem = aux->next;
		aux->next = torem->next;
		list->size--;
		return torem;
}

unsigned int add_ascending(dll_t *list, unsigned int address)
{
	unsigned int n = 0;
	// pornim de la inceputul listei
	dll_node_t *aux = list->head;

    // daca lista e goala, singura pozitie disponibila e prima
	if (!list->head) 
		return 0;
    //  daca adresa data e mai mica decat adresa primului nod, atunci toate
    // dupa vor fi mai mici, deci singura pozitie disponibila e prima
	else if (address < aux->address)
		return 0; // 

	while (aux->next && aux->next->address < address) {
		// se cauta ultima adresa mai mica decat adresa data
		aux = aux->next;
		n++;
	}
	// returnam pozitia imediat urmatoare
	return n + 1;
}

int find_node(dll_t *list, unsigned int address)
{
    // daca lista e goala, returnam -1 (eroare)
	if (!list->head)
		return -1;
	dll_node_t *aux = list->head;
	int n = 0;
	while (aux) {
		// se cauta nodul ce contine adresa data, returnandu-i pozitia
		if (aux->address == address)
			return n; 
		aux = aux->next;
		n++;
	}
	// daca nu s a gasit, returnam -1 (eroare)
	return -1;
}

dll_t *dll_create(unsigned int size)
{
	dll_t *list = malloc(1 * sizeof(dll_t));
    DIE(!list, "malloc");

	list->head = NULL;
	list->size = 0;
	list->bytes_size = size;
	return list;
}

void free_list(dll_t **list)
{
	dll_node_t *node = (*list)->head;
	while (node) {
		dll_node_t *aux = node;
		node = node->next;
		if (aux->data)
			free(aux->data);
		free(aux);
	}
	free(*list);
	*list = NULL;
}