#include <stdlib.h>
#include <unistd.h>
#include "tree.h"
 
#ifndef _QUEUE_H
#define _QUEUE_H

#define NUM_CHARS	256

struct s_pqueue 
{
	int heap_size;
	struct s_tree *A[NUM_CHARS];
};

#define PQSIZE (sizeof (struct s_pqueue))
 
void create_pq(struct s_pqueue *p);
void insert_pq(struct s_pqueue *p, struct s_tree *r);
int parent(int i);
int left(int i);
int right(int i);
struct s_tree *extract_min_pq(struct s_pqueue *p);
void heapify(struct s_pqueue *p, int i);

#endif
