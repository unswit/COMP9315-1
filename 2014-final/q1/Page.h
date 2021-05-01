// Page.h ... data structures for Pages

#ifndef PAGE_H
#define PAGE_H

#define TOTAL_BYTES_PER_PAGE  1024
#define HEADER_BYTES_PER_PAGE 256
#define TUPLE_BYTES_PER_PAGE  768
#define MAX_TUPLES_PER_PAGE   63


// Page object
// ntuples = count of the number of tuples currently in the page
// offsets = array of offsets in the page for each tuple
// tuples  = byte array containing '\0'-terminated strings, one per tuple

typedef struct _page {
	int  ntuples;
    int  offset[MAX_TUPLES_PER_PAGE];
	char tuples[TUPLE_BYTES_PER_PAGE];
} Page;

// Operations on whole Pages

void writePage(int file, int pageID, Page p);
int readPage(int file, int pageID, Page *p);

#endif
