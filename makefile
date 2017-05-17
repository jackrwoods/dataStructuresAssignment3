default: skipList

skipList: skipList.c skipList.h
	gcc -Wall -ansi skipList.c	

clean:
	rm *.o
