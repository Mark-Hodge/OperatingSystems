#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <pthread.h>
#include <sys/time.h>

#define LISTSIZE 10000

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

//																												------------------------List_Init()
void List_Init(list_t *L) {
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}

//																												------------------------List_Insert()
void List_Insert(list_t *L, int key) {

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
}

//																												------------------------List_Lookup()
int List_Lookup(list_t *L, int key) {
	int rv = -1;
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;
	while (curr) {
		if (curr->key == key) {
			rv = 0;
			break;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv; // now both success and failure
}

//																												------------------------Task_1()
void * Task_1(void *L) {
	printf("\t-> Task_1: begin (thread id = %ld)\n", pthread_self());
	
	for (int i = 0; i < (LISTSIZE / 2); i++) {
		int randomKey = rand();
		List_Insert(L, randomKey);
	}
	
	printf("\tTask_1: end (thread id = %ld)\n", pthread_self());
	return NULL;
}

//																												------------------------Task_2()
//																													 (1/2) Insert Step
void * Task_2Insert(void *L) {
	printf("\t-> Task_2 Insert: begin (thread id = %ld)\n", pthread_self());

	
	for (int i = 0; i < (LISTSIZE); i++) {
		int randomKey = rand();
		List_Insert(L, randomKey);
	}
	
	printf("\tTask_2 Insert: end (thread id = %ld)\n", pthread_self());
	return NULL;
}

//																													(2/2) Lookup Step
void * Task_2Lookup(void *L) {
	printf("\t-> Task_2 Lookup: begin (thread id = %ld)\n", pthread_self());

	for (int i = 0; i < (LISTSIZE); i++) {
		int randomKey = rand();
		List_Lookup(L, randomKey);
	}
	
	printf("\tTask_2 Lookup: end (thread id = %ld)\n", pthread_self());
	return NULL;
}

//																												------------------------Task_3()
void * Task_3(void *L) {
	printf("\t-> Task_3: begin (thread id = %ld)\n", pthread_self());

	for (int i = 0; i < (LISTSIZE); i++) {
		int randomKey = rand();
		List_Lookup(L, randomKey);
	}
	
	printf("\tTask_3: end (thread id = %ld)\n", pthread_self());
	return NULL;
}

int main(int argc, char *argv[]) {
	printf("\nHomework 3 - Performance Comparison of Thread-Safe Implementations of Linked List\n"
		"\tSample Size: %d\n\n", LISTSIZE);
	printf("main: begin (thread id = %ld)\n", pthread_self());
	
	// initialize time structs and variables to hold execution time of each task
	struct timeval start1, start2, start3;
	struct timeval end1, end2, end3;
	long t1_microsecs, t2_microsecs, t3_microsecs;
	
	// Seed the random number generator
	srand(time(0));
	
	// initialize new linked list structure
	//	this can be used for task 1 & 3 since it will already be populated for task 3
	list_t L;
	List_Init(&L);
	
	// initialize new linked list structure for tas 2
	list_t L2;
	List_Init(&L2);
	
	// initialize two threads to be used for the tasks
	pthread_t p1, p2;
	
	/*
		Task 1:
			Info: Starting with an empty list, two threads running at the same time insert
			1 million random integers each on the same list.
	*/
	gettimeofday(&start1, NULL);
	
	pthread_create(&p1, NULL, Task_1, &L);
	pthread_create(&p2, NULL, Task_1, &L);
	
	// wait for the threads to finish and join
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	
	// get time at end of task 1 and calculate execution time
	gettimeofday(&end1, NULL);
	t1_microsecs = ((end1.tv_sec*1e6 + end1.tv_usec) - (start1.tv_sec*1e6 + start1.tv_usec));
	printf("\n\t-------Time of Task 1: %ld microseconds-------\n\n", t1_microsecs);
	
	/*
		Task 2:
			Info: Starting with an empty list, one thread inserts 1 million random integers,
			while another thread looks up 1 million random integers at the same time.
	*/
	gettimeofday(&start2, NULL);
	
	pthread_create(&p1, NULL, Task_2Insert, &L2);
	pthread_create(&p2, NULL, Task_2Lookup, &L2);
	
	// wait for the threads to finish and join
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	
	// get time at end of task 2 and calculate execution time
	gettimeofday(&end2, NULL);
	t2_microsecs = ((end2.tv_sec*1e6 + end2.tv_usec) - (start2.tv_sec*1e6 + start2.tv_usec));
	printf("\n\t-------Time of Task 2: %ld microseconds-------\n\n", t2_microsecs);
	
	/*
		Task 3:
			Info: Starting with a list containing 1 million random integers, two threads running
			at the same time look up 1 million random integers each.
	*/
	gettimeofday(&start3, NULL);
	
	pthread_create(&p1, NULL, Task_3, &L);
	pthread_create(&p2, NULL, Task_3, &L);
	
	// wait for the threads to finish and join
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	
	// get time at end of task 3 and calculate execution time
	gettimeofday(&end3, NULL);
	t3_microsecs = ((end3.tv_sec*1e6 + end3.tv_usec) - (start3.tv_sec*1e6 + start3.tv_usec));
	printf("\n\t-------Time of Task 3: %ld microseconds-------\n\n", t3_microsecs);
	
	printf("main: end (thread id = %ld)\n", pthread_self());
}