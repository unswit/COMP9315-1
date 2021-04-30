// tsig.c ... functions on Tuple Signatures (tsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include <unistd.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

static Bits codeword(char *attr_val, int m, int k) {
    Bits ret = newBits(m);
    if (k > m) k = m;
	// unsetAllBits(ret);
	if (attr_val == NULL || strlen(attr_val) == 0 || attr_val[0] == '?') return ret;
	int  nbits = 0;   // count of set bits
    srandom(hash_any(attr_val, strlen(attr_val)));
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
	// unsetAllBits(ret);
	if (r->params.sigtype == 'c') {
		char **attval = tupleVals(r, t);
		int currbit = 0;
		for (i = 0 ; i < nAttrs(r); ++i) {
			int x = m / nAttrs(r) + (i == 0 ? m % nAttrs(r) : 0);
			Bits curr = codeword(attval[i], x, iceil(x, 2));
			for (j = 0 ; j < x; ++j) {
				if (bitIsSet(curr, j)) setBit(ret, currbit);
				currbit++;
			}
			freeBits(curr);
		}
		freeVals(attval, nAttrs(r));
		free(attval);
	} else {
		char **attval = tupleVals(r, t);
		for (i = 0 ; i < nAttrs(r); ++i) {
			Bits curr = codeword(attval[i], m, k);
			orBits(ret, curr);
			freeBits(curr);
		}
		freeVals(attval, nAttrs(r));
		free(attval);
	}
	
	return ret;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO
	unsetAllBits(q->pages);
	Bits curr = newBits(tsigBits(q->rel));
	Bits querysig = makeTupleSig(q->rel, q->qstring);
	int i, j;
	q->nsigpages = 0;
	q->nsigs = nTsigs(q->rel);
	for (i = 0 ; i < nTsigPages(q->rel); ++i) {
		Page tsig = getPage(q->rel->tsigf, i);
		q->nsigpages++;
		for (j = 0 ; j < pageNitems(tsig); ++j) {
			getBits(tsig, j, curr);
			if (isSubset(querysig, curr)) {
				PageID pid = (i * maxTsigsPP(q->rel) + j) / maxTupsPP(q->rel);
				setBit(q->pages, pid);
			}
		}
		free(tsig);
	}
	freeBits(curr);
	freeBits(querysig);
}

