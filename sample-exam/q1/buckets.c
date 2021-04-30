// buckets.c ... which buckets to examine
//
// Usage:  ./buckets  QueryHash
//
// The QueryHash value contains 0's, 1's and *'s and
// represents the multi-attribute hash value produced
// from a query like a,?,c
//
// Using bit-strings derived from this query hash, the
// program should print a list of buckets that will be
// examined in order to answer the query
//
// Bit-strings are written so that the most significant bit is
// on the left and the least significant bit is on the right.
//
// Example #1: consider a query "a,?,c" on a 32-page file
// where the choice vector is (0,0),(1,0),(2,0),(1,1),(0,1)
// assuming the query hash = 1*0*1  (depends on hash function)
// and the program would indicate buckets 17, 19, 25, 27
//
// Example #2: consider a query "x,y,?" on a 32-page file
// where the choice vector is (0,0),(1,0),(2,0),(1,1),(0,1)
// assuming the query hash = 01*01  (depends on hash function)
// and the program would indicate buckets 9, 13
//
// We do not consider hash functions, choice vectors or overflow
// pages in this question. This code is invoked after the bit
// strings are produced using the query, the hash function, and
// the choice vector

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bits.h"


int main(int argc, char **argv)
{
	int  i, j, nbits;
	Bits known, unknown;
	char out[40]; // output buffer for displaying bit-strings

	if (argc < 2) {
		fprintf(stderr, "Usage:  ./buckets  QueryHash\n");
		exit(EXIT_FAILURE);
	}

	nbits = strlen(argv[1]);
	assert(nbits > 0 && nbits < 32);

	// set up known and unknown bit-strings

	known = zeroBits(nbits);  unknown = zeroBits(nbits);

	j = nbits-1;
	unsigned int k = 0;
	int cnt = 0;
	for (i = 0; i < nbits; i++,j--) {
		char c = argv[1][i];
		if (c == '1')
			known = setBit(known,j);
		else if (c == '0') {
			/* nothing to do */
		}
		else if (c == '*') {
			unknown = setBit(unknown,j);
			cnt++;
		}
		else {
			fprintf(stderr, "Invalid QueryHash\n");
			exit(EXIT_FAILURE);
		}
	}

	showBits(known,out); printf("Known:   %s\n", out);
	showBits(unknown,out); printf("Unknown: %s\n", out);

	// calculate buckets to be examined
	// TODO: add your code here
    for (k = 0 ; k < (1 << cnt); ++k) {
		Bits curr = known;
		for (i = 0 ; i < cnt; ++i) {
			if (k & (1 << i)) {
				int tol = 0;
				for (j = 0; j < nBits(unknown); ++j) {
					if (bitIsSet(unknown, j) && tol == i) {
						tol = j;
						// printf("we find tol= %d\n", tol);
						break;
					}

					if (bitIsSet(unknown, j)) {
						tol++;
					}
				}
                curr = setBit(curr, tol);
			}
		}
		printf("%u\n", bitsToInt(curr));
	}
	return EXIT_SUCCESS;
}
