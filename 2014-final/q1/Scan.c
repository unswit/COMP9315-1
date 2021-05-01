// Scan.c ... scanning operations

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Globals.h"
#include "Scan.h"
#include "Page.h"

// startScan: start a scan on an open file
// - returns a pointer to a newly created Scan object
// - if Scan object can't be created, return NULL
// - if Scan object is created, initialise it appropriately
//   - which includes making a copy of the file descriptor
Scan *startScan(int file)
{
	Scan *ret = malloc(sizeof(Scan));
	if (ret == NULL) return NULL;
	ret->file = file;
	ret->cur_tup = -1;
	ret->cur_page = 0;
	readPage(file, ret->cur_page, &ret->page);
	return ret; // replace this line
}

// nextTuple: get the tuple during a scan
// - if no more tuples, return NONE
// - if finds an invalid tuple offset, return NONE
// - otherwise advance to next tuple
//   - may have to move to next page
//   - may have to skip empty pages
//   - may eventually discover "no more tuples"
// - copy next tuple string into buf and return OK
int nextTuple(Scan *s, char *buf)
{
	if (s == NULL) return NONE;
	int ntuple = s->page.ntuples;
	s->cur_tup++;
	while (ntuple == s->cur_tup || ntuple == 0) {
		s->cur_tup = 0;
		s->cur_page++;
		if (readPage(s->file, s->cur_page, &s->page) != OK) {
			return NONE;
		}
		ntuple = s->page.ntuples;
	}
	// printf("%d %d\n", ntuple, s->page.ntuples);
	if (ntuple == 0) return NONE;
	int offset = s->page.offset[s->cur_tup];
	int len = strlen(s->page.tuples + offset);
	// buf = calloc(len+1, sizeof(char));
	if (buf == NULL) return NONE;
	memcpy(buf, s->page.tuples + offset, len);
	buf[len] = '\0';
	return OK; // replace this line
}

// closeScan: clean up after a scan finishes
void closeScan(Scan *s)
{
	if (s != NULL) free(s);
}
