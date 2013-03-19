#include "pqueue.h"

void create_pq(struct s_pqueue *p) 
{
	p->heap_size = 0;
}

void insert_pq(struct s_pqueue *p, struct s_tree *r) 
{
	int i;

	p->heap_size++;
	i = p->heap_size - 1;
	while ((i > 0) && (p->A[parent(i)]->freq >= r->freq)) 
	{
		p->A[i] = p->A[parent(i)];
		i = parent(i);
	}
	p->A[i] = r;
}

int parent(int i) 
{
	return (i - 1) / 2;
}

int left(int i) 
{
	return i * 2 + 1;
}

int right(int i) 
{
	return i * 2 + 2;
}

struct s_tree *extract_min_pq(struct s_pqueue *p) 
{
	struct s_tree *r = NULL;

	if (p->heap_size == 0) 
	{
		fprintf(stderr, "heap underflow!\n");
		exit(EXIT_FAILURE);
	}
	r = p->A[0];
	p->A[0] = p->A[p->heap_size - 1];
	p->heap_size--;
	heapify(p, 0);
	return r;
}

void heapify(struct s_pqueue *p, int i) 
{
	int l;
	int r;
	int smallest;
	struct s_tree *t;


	l = left(i);
	r = right(i);
	if (l < p->heap_size && p->A[l]->freq <= p->A[i]->freq) 
		smallest = l;
	else
		smallest = i;
	if (r < p->heap_size && p->A[r]->freq <= p->A[smallest]->freq)
		smallest = r;
	if (smallest != i) 
	{
		t = p->A[i];
		p->A[i] = p->A[smallest];
		p->A[smallest] = t;
		heapify(p, smallest);
	}
}
