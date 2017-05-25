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
	/*int i,j, M;*/
	struct skipList * slst = (struct skipList *) malloc(sizeof(struct skipList));
	struct skipList * slst2 = (struct skipList *) malloc(sizeof(struct skipList));

	srand ( time(NULL) );

	/*  Initialize the skip list */
	initSkipList(slst);
	initSkipList(slst2);

	/* ensure that memory was allocated correctly. */
	assert(slst);
	assert(slst2);

	/*  Add to the skip list  M = 20 random integers in [0,100] */
	int iter;
	for(iter = 0; iter < 20; iter++) {
		addSkipList(slst, (double) (rand() % 100));
	}

	/* Print items from the skiplist */
	printSkipList(slst);
	printf("\n-----\n\n");

	/* Add three elements to the skip list, print the skip list, remove the elements,
	 * and then print the skip list again to verify that the functions work. */
	double valuesToRemove[3];
	for (iter = 0; iter < 3; iter++) { /* iter was previously declared, just re-using it */
	  valuesToRemove[iter] = (double) (rand() % 100);
	  addSkipList(slst, valuesToRemove[iter]);
		addSkipList(slst2, valuesToRemove[iter]);
	}

	printSkipList(slst);
	printf("\n-----\n\n");

	for (iter = 0; iter < 3; iter++) {
		removeSkipList(slst, valuesToRemove[iter]);
	}

	printSkipList(slst);
	printf("\n-----\n\n");

	printf("Contains results are %d, %d, and %d.\n", containsSkipList(slst, valuesToRemove[0]), containsSkipList(slst, valuesToRemove[1]), containsSkipList(slst, valuesToRemove[2]));

	mergeSkipList(slst, slst2);

	printSkipList(slst);
	printf("\n-----\n\n");

	printf("Contains results are %d, %d, and %d.\n", containsSkipList(slst, valuesToRemove[0]), containsSkipList(slst, valuesToRemove[1]), containsSkipList(slst, valuesToRemove[2]));

	 deleteSkipList(slst);
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
	return (rand() % 2);
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
	struct skipLink * next = current->next; /* Store next pointer for new element */
	if (current->down != NULL) current->next = newSkipLink(e, next, skipLinkAdd(slideRightSkipList(current->down, e), e));
	else current->next = newSkipLink(e, next, NULL);
	assert(current->next); /* Ensure the element's memory was allocated correctly */
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
	int num_rows = 0;
	while ((rand() % 2) > 0) {/* 50% chance that the element will appear in a given row */
		num_rows++;
	}

	int rows = 0;
	struct skipLink * current = slst->topSentinel;
	while (current->down != NULL) {
		rows++;
		current = current->down;
	}

	/* Add additional rows if needed.*/
	int num_rows_iter = num_rows;
	while (num_rows_iter > rows) {
		current = slst->topSentinel;
		slst->topSentinel = newSkipLink(0, NULL, current);
		num_rows_iter--;
	}

	if (num_rows > rows) rows = num_rows;

	current = slst->topSentinel;
	while (num_rows > rows) {
		current = current->down;
		num_rows--;
	}
	current = slideRightSkipList(current, e);
	skipLinkAdd(current, e);
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

	struct skipLink * current = slst->topSentinel;
	struct skipLink * beginning = slst->topSentinel;
	/* Iterate through each level of the skip list. */
	while (current != NULL) {

		/* Iterate over each element in the current row. */
		while (current->next != NULL) {
			current = current->next;
			printf(" %2.f ", current->value); /* The value is of TYPE double, given in skipList.h */
		}
		printf("\n");

		beginning = beginning->down;
		current = beginning;

	}

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
	/* Choose the bottom sentinel. */
	struct skipLink * bottomLink = slst2->topSentinel;
	while (bottomLink->down != NULL) {
		bottomLink = bottomLink->down;
	}
	/* Iterate over each element in the bottom row, and delete them all. */
	while (bottomLink->next != NULL) {
		/* Add element to slst1 */
		struct skipLink * next = bottomLink->next->next;
		addSkipList(slst1, bottomLink->next->value);
		/* Remove element from slst2 */
		removeSkipList(slst2, bottomLink->next->value);
		bottomLink->next = next;
	}

	/* Free the remaining memory associated with slst2. */
	deleteSkipList(slst2);

} /* end of the function */

/* Free all memory associated with the skipList. */
void deleteSkipList(struct skipList * slst) {
	/* Choose the bottom sentinel. */
	struct skipLink * bottomLink = slst->topSentinel;
	while (bottomLink->down != NULL) {
		bottomLink = bottomLink->down;
	}

	/* Iterate over each element in the bottom row, and delete them all. */
	struct skipLink * next = bottomLink;
	while (bottomLink->next != NULL) {
		/* Remove element from slst */
		next = bottomLink->next->next;
		removeSkipList(slst, bottomLink->next->value);
		bottomLink->next = next;
	}
	bottomLink = slst->topSentinel;
	while (bottomLink != NULL) {
		next = bottomLink->down;
		free(bottomLink);
		bottomLink = next;
	}
	/* removeSkipList will automatically reconnect bottomLink with bL->next->next. */

	/* Free the remaining memory associated with slst. */
	free(slst);
}

/* JW - Adding main function just for my own testing purposes. */

int main(int argc, char* argv[]) {
	test();
	return 0;
}
