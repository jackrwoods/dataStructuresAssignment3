#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "skipList.h"
#include <time.h>

/* ************************************************************************
Main Function
 ************************************************************************ */
/* Test function:
 param: no parameters
 pre:	no parameres
 post: prints out the contents of the skip list */

void test(){
	int i,j;
	int M;
	struct skipList *slst;
	test();

	srand ( time(NULL) );


	slst=(struct skipList *)malloc(sizeof(struct skipList));  /*2*/
	assert(slst);



	/*  Initialize the skip list */

  slst->topSentinel = (struct skipLink *)malloc(sizeof(struct skipLink));
	assert (slst->topSentinel);

	/*  Add to the skip list  M = 20 random integers in [0,100] */

	for(int i = 0; i < 20; i++) {
		addSkipList(slst, (double) rand() % 100);
	}

	 	struct skipLink * current = slst->topSentinel;
		struct skipLink * beginning = slst->topSentinel;
		// Iterate through each level of the skip list.
		while (current->down != NULL) {

			// Iterate over each element in the current row.
			while (current->next != NULL) {
				current = current->next;
				printf("  %f  ", current->value); // The value is of TYPE double, given in skipList.h
			}

			beginning = beginning->down;
			current = beginning;

		}



	/* Develop 3 test cases for printing out the contents of skip list
           after remove function:
	         int removeSkipList(struct skipList *slst, TYPE e)
	 */



        /*  FIX ME */





         /* Develop 3 test cases corresponding to the above 3 test cases
            for printing out the output of contains function when e is equal to
            the elements you removed above:
	          int containsSkipList(struct skipList *slst, TYPE e)
	 */


        /*  FIX ME */


}


/* ************************************************************************
Internal Functions
 ************************************************************************ */

/* Coin toss function:
 param: 	no parameters
 pre:	no parameres
 post: output is a random intiger number in {0,1} */
int flipSkipLink(void)
{
	return rand() % 2;
}

/* Move to the right as long as the next element is smaller than the input value:
 param: 	current -- pointer to a place in the list from where we need to slide to the right
 param:	e --  input value
 pre:	current is not NULL
 post: returns one link before the link that contains the input value e */
struct skipLink * slideRightSkipList(struct skipLink *current, TYPE e){
	while ((current->next != 0) && LT(current->next->value, e))
		current = current->next;
	return current;
}


/* Create a new skip link for a value
	param: e	 -- the value to create a link for
	param: nextLnk	 -- the new link's next should point to nextLnk
	param: downLnk -- the new link's down should point to downLnk
	pre:	none
	post:	a link to store the value */
struct skipLink* newSkipLink(TYPE e, struct skipLink * nextLnk, struct skipLink* downLnk) {
	struct skipLink * tmp = (struct skipLink *) malloc(sizeof(struct skipLink));
	assert(tmp != 0);
	tmp->value = e;
	tmp->next = nextLnk;
	tmp->down = downLnk;
	return tmp;
}


/* Add a new skip link recursively
 param: current -- pointer to a place in the list where the new element should be inserted
 param: e	 -- the value to create a link for
 pre:	current is not NULL
 post: a link to store the value */
struct skipLink* skipLinkAdd(struct skipLink * current, TYPE e) {
	slideRightSkipList(current, e); /* slideRightSkipList returns the element to
																	 * the right of where the element should be
																	 * inserted.*/
	struct skipLink * next = current->next; // Store next pointer for new element
	current->next = (struct skipLink *)malloc(sizeof(struct skipLink));
	assert(current->next); // Ensure the element's memory was allocated correctly
	current->next->next = next;
	if (current->down != NULL) current->next->down = skipLinkAdd(current, e);
	else current->next->down = NULL;
	return current->next;
}


/* ************************************************************************
Public Functions
 ************************************************************************ */

/* Initialize skip list:
 param:  slst -- pointer to the skip list
 pre:	slst is not null
 post: the sentinels are allocated, the pointers are set, and the list size equals zero */
void initSkipList (struct skipList *slst)
{

	slst->size=0;
	slst->topSentinel=(struct skipLink * )malloc(sizeof(struct skipLink));
	assert(slst->topSentinel);
	slst->topSentinel->next=NULL;
	slst->topSentinel->down=NULL;
}

/* Checks if an element is in the skip list:
 param: slst -- pointer to the skip list
 param: e -- element to be checked
 pre:	slst is not null
 post: returns true or false  */
int containsSkipList(struct skipList *slst, TYPE e)
{
	struct skipLink *tmp = slst->topSentinel;
	while (tmp != 0) {  /* 2pts */
		tmp = slideRightSkipList(tmp, e); /* 1pt */
		if ((tmp->next != 0) && EQ(e, tmp->next->value)) /* 2pts */
			return 1;
		tmp = tmp->down;  /* 1pt */
	}
	return 0;
}


/* Remove an element from the skip list:
 param: slst -- pointer to the skip list
 param: e -- element to be removed
 pre:	slst is not null
 post: the new element is removed from all internal sorted lists */
void removeSkipList(struct skipList *slst, TYPE e)
{
	struct skipLink *current, *tmp;
	current = slst->topSentinel; /* 1pts */
	while (current != 0) {  /* 2pts */
		current = slideRightSkipList(current, e);
		if (current->next != 0 && EQ(e, current->next->value)) {  /* 2pts */
			tmp = current->next; /* 2pts */
			current->next = current->next->next;
			free(tmp);    /* 2pts */
			if (current->down == NULL)
				slst->size--;   /* 2pts */
		}
		current = current->down; /* 1pt */
	}
}




/* Add a new element to the skip list:
	param: slst -- pointer to the skip list
	param: e -- element to be added
	pre:	slst is not null
	post:	the new element is added to the lowest list and randomly to higher-level lists */
void addSkipList(struct skipList *slst, TYPE e)
{
	/* Flip a coin for each row in the list. */
	int num_rows = 1; /* The first row is given. */
	while (rand % 2 > 0) {/* 50% chance that the element will appear in a given row */
		num_rows++;
	}

	int rows_present = 1;/* This contains the number of rows that currently exist
												* in the skip list. If we roll a higher num_rows, then
												* each subsequent row should be created. */
	struct skipLink* firstElement = slst->topSentinel;
	while (firstElement->down != NULL) {
		rows_present++;
	}

	firstElement = slst->topSentinel;/* Reset the element to the current top. */
	while (num_rows > rows_present) {
		slst->topSentinel = (struct skipLink *)malloc(sizeof(struct skiplink));
		assert(slst->topSentinel);
		slst->topSentinel->down = firstElement;/* The old top is placed below the new one. */
		firstElement = slst->topSentinel;/*Reset pointer to top element*/
		rows_present--;
	}

	slst->topSentinel->next = skipLinkAdd(firstElement, e);
}

/* Find the number of elements in the skip list:
 param: slst -- pointer to the skip list
 pre:	slst is not null
 post: the number of elements */
int sizeSkipList(struct skipList *slst){

	return slst->size;

}


/* Print the links in the skip list:
	param: slst -- pointer to the skip list
	pre:	slst is not null and slst is not empty
	post: the links in the skip list are printed breadth-first, top-down */
void printSkipList(struct skipList *slst)
{

	/* FIX ME*/

}


/* Merge two skip lists, by adding elements of skip list 2 to skip list 1
 that are not already contained in skip list 1.
 The function is also supposed to remove the entire skip list 2 from the memory.
 param: slst1 -- pointer to the skip list 1
 param: slst2 -- pointer to the skip list 2
 pre: slst1 and slst2 are not null, and skip list 1 and skip list 2 are not empty
 post: slst1 points to the merger skip list,  slst2 is null*/
void mergeSkipList(struct skipList *slst1, struct skipList *slst2)
{

	/* FIX ME*/

} /* end of the function */
