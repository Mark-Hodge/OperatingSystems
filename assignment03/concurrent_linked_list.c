#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <pthread.h>
#include <sys/time.h>

#define LISTSIZE 100

/* typedef struct __list_args {
	int key[LISTSIZE / 2];
	void *L;
} list_args; */

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
	//printf("\t-> List_Insert: begin (thread id = %ld)\n", pthread_self());

	// synchronization not needed
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("malloc");
		return;
	}
	new->key = rand();
	
	// just lock critical section
	pthread_mutex_lock(&L->lock);
	new->next = L->head;
	L->head = new;
	pthread_mutex_unlock(&L->lock);
	
	//printf("\t   key: %d next: %p\n", new->key, new->next);
	
	//printf("\t   List_Insert: end (thread id = %ld)\n", pthread_self());
}
/* 
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
} */

void * Task_1(void *L) {
	printf("\t-> Task_1: begin (thread id = %ld)\n", pthread_self());
/* 	list_t L;
	List_Init(&L); */
	
	int randomSeed = 0;
	srand(randomSeed);
	
	for (int i = 0; i < (LISTSIZE / 2); i++) {
		int randomKey = rand();
		List_Insert(L, randomKey);
	}
	
	printf("\tTask_1: end (thread id = %ld)\n", pthread_self());
	return NULL;
}

int main(int argc, char *argv[]) {
	printf("main: begin (thread id = %ld)\n", pthread_self());
	
	struct timeval start1;
	struct timeval end1;
	long t1_microsecs;
	
	// initialize new linked list structure
	list_t L;
	List_Init(&L);
	
	// create two new threads and call List_Insert
	pthread_t p1, p2;
	gettimeofday(&start1, NULL);
	
	pthread_create(&p1, NULL, Task_1, &L);
	pthread_create(&p2, NULL, Task_1, &L);
	
	// wait for the threads to finish and join
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	
	gettimeofday(&end1, NULL);
	
	printf("main: end (thread id = %ld)\n", pthread_self());
	t1_microsecs = (end1.tv_usec - start1.tv_usec);
	printf("\n\t-------Time of Task 1: %ld microseconds-------\n\n", t1_microsecs);
}