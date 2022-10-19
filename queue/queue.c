/* 
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>

#include "harness.h"
#include "queue.h"

/*
   Create empty queue.
   Return NULL if could not allocate space.
   */
queue_t *q_new()
{
	queue_t *q =  malloc(sizeof(queue_t));
	/* What if malloc returned NULL? */
	if( q == NULL )
	{
		return NULL;
	}
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
	if (q == NULL)
	{
		return;
	}
	while( q->head != NULL && q != NULL)
	{
		list_ele_t *t; 
		t = q->head;
		q->head = q->head->next;
		free(t);
	}
	/* Free queue structure */
	free(q);

}

/*
   Attempt to insert element at head of queue.
   Return true if successful.
   Return false if q is NULL or could not allocate space.
   */
bool q_insert_head(queue_t *q, int v)
{
	list_ele_t *newh;
	if( q == NULL)
	{
		return false;
	}
	newh = malloc(sizeof(list_ele_t));
	if( newh == NULL )
	{
		return false;
	}
	newh->value = v;
	newh->next = q->head;
	if(q->head != NULL) q->head->prev = newh;
	q->head = newh;
	q->size = q->size + 1;
	if( q->size == 1)
	{
		q->tail = newh;
		q->tail->next = NULL;
	}
	q->head->prev = NULL;
	return true;
	
}


/*
   Attempt to insert element at tail of queue.
   Return true if successful.
   Return false if q is NULL or could not allocate space.
   */
bool q_insert_tail(queue_t *q, int v)
{
	list_ele_t *newt;
	if( q == NULL )
	{
		return false;
	}
	newt = malloc(sizeof(list_ele_t));
	if( newt == NULL )
	{
		return false;
	}
	if( q->size == 0 )
	{
		q->head = newt;
		q->tail = newt;
		q->head->prev = NULL;
	}
	else
	{
		q->tail->next = newt;
		newt->prev = q->tail;
		q->tail = newt;
	}
	newt->value = v;
	q->tail->next = NULL;
	q->size = q->size + 1;
	return true;
}

/*
   Attempt to remove element from head of queue.
   Return true if successful.
   Return false if queue is NULL or empty.
   If vp non-NULL and element removed, store removed value at *vp.
   Any unused storage should be freed
   */
bool q_remove_head(queue_t *q, int *vp)
{
	if( q == NULL || q->size == 0)
	{
		return false;
	}
	list_ele_t *t = q->head;
	t->value = q->head->value;
	q->head = q->head->next;
	if( vp != NULL )
	{
		*vp = t->value;
	}
	free(t);
	q->size = q->size - 1;
	if (q->size != 0)
		q->head->prev = NULL;
	return true;
}

/*
   Return number of elements in queue.
   Return 0 if q is NULL or empty
   */
int q_size(queue_t *q)
{
	if(q == NULL || q->size == 0 )
	{
		return 0;
	}

	return q->size;
}

/*
   Reverse elements in queue.

   Your implementation must not allocate or free any elements (e.g., by
   calling q_insert_head or q_remove_head).  Instead, it should modify
   the pointers in the existing data structure.
   */
void q_reverse(queue_t *q)
{
	if(q == NULL)
	{
		return;
	}
	/*
	list_ele_t *prev = NULL;
	list_ele_t *cur = q->head;
	list_ele_t *next = NULL;
	list_ele_t *temp = NULL;
	temp = q->tail;
	q->tail = q->head;
	q->head = temp;
	
	while( cur != NULL) 
	{
		Reverses the direction of the pointers
		next = cur->next;
		cur->next = prev;
		prev = cur;
		cur = next;
	}
	*/
	list_ele_t *temp = q->head;
	q->head = q->tail;
	q->tail = temp;
	list_ele_t *cur = q->tail;
	while(cur != NULL)
		{
			list_ele_t *t = cur->prev;
			cur->prev = cur->next;
			cur->next = t;
			cur = cur->prev;
		}
		q->head->prev = NULL;
		q->tail->next = NULL;
}
/*Removes the tail */
bool q_remove_tail(queue_t *q, int *vp){
		if( q == NULL || q->size == 0)
	{
		return false;
	}
	list_ele_t *t = q->tail;
	t->value = q->tail->value;
	q->tail = q->tail->prev;
	if( vp != NULL )
	{
		*vp = t->value;
	}
	free(t);
	q->size = q->size - 1;
	if(q->size != 0)
		q->tail->next = NULL;
	return true;
}

