/*
COMP9315 14s2 Final Exam

Display all tuples from a table stored in a file
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "Scan.h"
#include "Page.h"

#define MAX_TUPLE_LENGTH 200

int main(int argc, char **argv)
{
	int file;
	char tuple[MAX_TUPLE_LENGTH+1];  // input line buffer
	
	// deal with command-line parameters
	if (argc < 2) {
		fprintf(stderr,"Usage:  %s File\n",argv[0]);
		exit(1);
	}
	if ((file = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr,"Cannot open %s\n",argv[1]);
		exit(1);
	}

	// run the scan
	Scan *s;
	if ((s = startScan(file)) == NULL) {
		fprintf(stderr,"Cannot start scan\n");
		exit(1);
	}
	while (nextTuple(s,tuple) != NONE) {
		printf("%s   (tuple %d from page %d)\n", tuple, s->cur_tup, s->cur_page);
	}
	closeScan(s);

	// finish up
	return EXIT_SUCCESS;
}
