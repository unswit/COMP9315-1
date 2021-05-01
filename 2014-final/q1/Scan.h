// Scan.c ... scanning operations

#ifndef SCAN_H
#define SCAN_H

#include <stdlib.h>
#include <unistd.h>
#include "Globals.h"
#include "Scan.h"
#include "Page.h"

// Scan object
// file     = opened file (containing tuples)
// cur_page = page ID of current page 0..b-1
// cur_tup  = tuple ID of current tuple 1..ntuples
// page     = buffer to hold current page

typedef struct _scan {
	int  file;
	int  cur_page;
	int  cur_tup;
	Page page;
} Scan;

// Operations on Scans

// initialise a scan on an open file
Scan *startScan(int file);
// get the next tuple during a scan
int nextTuple(Scan *s, char *buf);
// clean up after a scan finishes
void closeScan(Scan *s);

#endif
