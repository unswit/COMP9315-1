// tsig.c ... functions on Tuple Signatures (tsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

static Bits codeword(char *attr_val, int m, int k) {
    Bits ret = newBits(m);
	unsetAllBits(ret);
	if (attr_val == NULL || strlen(attr_val) == 0 || attr_val[0] == '?') return ret;
	int  nbits = 0;   // count of set bits
    srandom(hash_any(attr_value, strlen(attr_value)));
    while (nbits < k) {
		int i = random() % m;
		if (!bitIsSet(ret, i)) {
			setBit(ret, i);
			nbits++;
		}
	}
   	return ret;
}

// make a tuple signature
Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	int i, j, m = tsigBits(r), k = codeBits(r);
	Bits ret = newBits(m);
	unsetAllBits(ret);
	if (r->params.sigtype == 'c') {
		char **attval = tupleVals(r, t);
		int currbit = 0;
		for (i = 0 ; i < nAttrs(r); ++i) {
			int x = m / nAttrs(r) + (i == 0 ? m % nAttrs(r));
			Bits curr = codeword(attval, x, x / 2);
			for (j = 0 ; j < x; ++j) {
				if (bitIsSet(curr, j)) setBit(ret, currbit);
				currbit++;
			}
			freeBits(curr);
		}
		freeVals(attval, nAttrs(r));
	} else {
		char **attval = tupleVals(r, t);
		for (i = 0 ; i < nAttrs(r); ++i) {
			Bits curr = codeword(attval, m, k);
			ret = orBits(ret, curr);
			freeBits(curr);
		}
		freeVals(attval, nAttrs(r));
	}

	return ret;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO
	unsetAllBits(q->pages);
	Bits querysig = makeTupleSig(q->rel, q->qstring);
	int i, j;
	PageID p = 0, itemid = 0;
	for (i = 0 ; i < nTsigPages(q->rel); ++i) {
		Page tsig = getPage(q->rel->tsigf, i);
		for (j = 0 ; j < pageNitems(tsig); ++j) {
			Bits curr = newBits(q->rel->params.tm);
			getBits(tsig, j, curr);
			if (isSubset(curr, querysig) && isSubset(querysig, curr)) {
				PageID pid = p;
				setBit(q->pages, pid);
			}
			freeBits(curr);
			itemid++;
			Page cur = getPage(q->rel->dataf, p);
			if (itemid == pageNitems(cur)) {
				p++;
				itemid = 0;
			}
		}
	}
	// The printf below is primarily for debugging
	// Remove it before submitting this function
	//printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}
