// psig.c ... functions on page signatures (psig's)
// part of signature indexed files
// Written by John Shepherd, March 2019
#include <math.h>
#include "defs.h"
#include "reln.h"
#include "query.h"
#include "hash.h"
#include "psig.h"
#include "util.h"

static Bits codeword(char *attr_val, int m, int k) {
    Bits ret = newBits(m);
    if (k > m) k = m;
	//unsetAllBits(ret);
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

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	int i, j, mp = psigBits(r), k = codeBits(r);
	char **vals = tupleVals(r, t);
	Bits ret = newBits(mp);
	//unsetAllBits(ret);
	if (sigType(r) == 's') {
	    for (i = 0 ; i < nAttrs(r); ++i) {
			Bits curr = codeword(vals[i], mp, k);
	        orBits(ret, curr);
			freeBits(curr);
		}
	} else {
		int currbit = 0;
		for (i = 0 ; i < nAttrs(r); ++i) {
			int x = mp / nAttrs(r) + (i == 0 ? mp % nAttrs(r) : 0);
			Bits curr = codeword(vals[i], x, iceil(x, (2 * maxTupsPP(r))));
			for (j = 0 ; j < x; ++j) {
				if (bitIsSet(curr, j)) setBit(ret, currbit);
				currbit++;
			}
			freeBits(curr);
		}
	}

	freeVals(vals, nAttrs(r));
	free(vals);
	return ret;
}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	unsetAllBits(q->pages);
	Bits querysig = makePageSig(q->rel, q->qstring);
	Bits curr = newBits(psigBits(q->rel));
	int i, j;
	q->nsigpages = 0;
	q->nsigs = 0;
	for (i = 0 ; i < nPsigPages(q->rel); ++i) {
		Page psig = getPage(q->rel->psigf, i);
		for (j = 0 ; j < pageNitems(psig); ++j) {		
			getBits(psig, j, curr);
			if (isSubset(querysig, curr)) {
				setBit(q->pages, q->nsigs);
			}
			q->nsigs++;
		}
		free(psig);
		q->nsigpages++;
	}
	freeBits(curr);
	freeBits(querysig);
}

