// Page.c ... operations on whole pages

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Globals.h"
#include "Page.h"

void writePage(int file, int pageID, Page p)
{
	ssize_t written;
	size_t offset = pageID * sizeof(Page);
	lseek(file, offset, SEEK_SET);
	written = write(file, &p, sizeof(Page));
	if (written != sizeof(Page)) {
		fprintf(stderr, "Writing page %d failed\n",pageID);
		exit(1);
	}
}

int readPage(int file, int pageID, Page *p)
{
	ssize_t bytes_read;
	size_t offset = pageID * sizeof(Page);
	lseek(file, offset, SEEK_SET);
	bytes_read = read(file, p, sizeof(Page));
	return (bytes_read == sizeof(Page)) ? OK : NONE;
}
