// bsig.c ... functions on Tuple Signatures (bsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"

void findPagesUsingBitSlices(Query q)
{
	int i, j, pre = -1;
	assert(q != NULL);
	setAllBits(q->pages); 
	Bits curr = makePageSig(q->rel, q->qstring);
	q->nsigs = 0;
	q->nsigpages = 0;
	for (i = 0 ; i < psigBits(q->rel); ++i) {
		if (bitIsSet(curr, i)) {
			int pid = i / maxBsigsPP(q->rel), offset = i % maxBsigsPP(q->rel);
			if (pre != pid) {
				pre = pid;
				q->nsigpages++;
			}
			Page p = getPage(q->rel->bsigf, pid);
			Bits cur = newBits(bsigBits(q->rel));
			// get the bit slice if the query signature vas this bit set to 1
			getBits(p, offset, cur);
			// we increment the number of signature read by 1
		    q->nsigs++;
			// we do the and operation to q->pages, note that the size are different
			for (j = 0 ; j < nPages(q->rel); ++j) {
				if (!bitIsSet(cur, j)) unsetBit(q->pages, j);
			}
			freeBits(cur);
			free(p);
		}
	}
	freeBits(curr);
}

