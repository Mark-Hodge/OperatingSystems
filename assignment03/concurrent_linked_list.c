#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// basic node structure
typedef struct __node_t {
	int key;
	struct __node_t *next;
} node_t;

// basic list structure
typedef struct __list_t {
	node_t *head;
	pthread_mutex_t lock;
} list_t;

void List_Init(list_t *L) {
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {
	// synchronization not needed
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("malloc");
		return;
	}
	new->key = key;
	
	// just lock critical section
	pthread_mutex_lock(&L->lock);
	new->next = L->head;
	L->head = new;
	pthread_mutex_unlock(&L->lock);
	
	printf("\t-> key: %d next: %p\n", key, new->next);
}

int List_Lookup(list_t *L, int key) {
	int rv = -1;
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;
	while (curr) {
		if (curr->key == key) {
			rv = 0;
			printf("\t-> key: %d next: %p\n", key, curr->next); 
			break;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv; // now both success and failure
}

int main(int argc, char *argv[]) {
	//pthread_t p1, p2;
	printf("main: begin\n");
	
	printf("main: list_init\n");
	list_t L;
	List_Init(&L);
	printf("\t-> address: %p\n", &L);
	
	printf("main: random seed\n");
	int randomSeed = 0;
	srand(randomSeed);
		printf("\t-> %d\n", randomSeed);
	int arr[10];

	
	printf("main: generate random keys\n");
	for (int i = 0; i < 10; i++) {
		int randomKey = rand();
		arr[i] = randomKey;
		printf("\t-> array[%d]: %d\n", i, randomKey);
	}
	
/* 	for (int i = 0; i < 10; i++) {
		printf("%d, ", arr[i]);
	} printf("\n"); */
	
	printf("main: loop init  list_insert\n");
	for (int i = 0; i < 10; i++) {
		List_Insert(&L, arr[i]);
	}
	
	printf("main: loop init list_lookup\n");
	for (int i = 0; i < 10; i++) {
		List_Lookup(&L, arr[i]);
	}
	
	printf("main: end\n");
}