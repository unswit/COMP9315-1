// bsig.c ... functions on Tuple Signatures (bsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
	setAllBits(q->pages); // remove this
}

