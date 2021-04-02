// bits.c ... functions on bit-strings
// part of signature indexed files
// Bit-strings are arbitrarily long byte arrays
// Least significant bits (LSB) are in array[0]
// Most significant bits (MSB) are in array[nbytes-1]

// Written by John Shepherd, March 2019

#include <assert.h>
#include "defs.h"
#include "bits.h"
#include "page.h"

typedef struct _BitsRep {
	Count  nbits;		  // how many bits
	Count  nbytes;		  // how many bytes in array
	Byte   bitstring[1];  // array of bytes to hold bits
	                      // actual array size is nbytes
} BitsRep;

static int getchunk(int position) {
	return position >> 3;
}

static int getbitpos(int position) {
	return position & 7;
}

// create a new Bits object

Bits newBits(int nbits)
{
	Count nbytes = iceil(nbits,8);
	Bits new = malloc(2*sizeof(Count) + nbytes);
	new->nbits = nbits;
	new->nbytes = nbytes;
	memset(&(new->bitstring[0]), 0, nbytes);
	return new;
}

// release memory associated with a Bits object

void freeBits(Bits b)
{
	free(b);
}

// check if the bit at position is 1

Bool bitIsSet(Bits b, int position)
{
	int chunk = getchunk(position), pos = getbitpos(position);
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	//TODO
	if ((b->bitstring[chunk] & (1 << pos)) > 0) {
		return TRUE;
	}
    return FALSE;
}

// check whether one Bits b1 is a subset of Bits b2

Bool isSubset(Bits b1, Bits b2)
{
	int i;
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	// TODO
	for (i = 0 ; i < b1->nbytes; ++i) {
		if ((b1->bitstring[i] & b2->bitstring[i]) != b1->bitstring[i]) return FALSE;
	}
	return TRUE;
}

// set the bit at position to 1

void setBit(Bits b, int position)
{
	int chunk = getchunk(position), pos = getbitpos(position);
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	b->bitstring[chunk] |= (1 << pos);
}

// set all bits to 1

void setAllBits(Bits b)
{
	int i;
	assert(b != NULL);
	//TODO
	for (i = 0 ; i < b->nbits; ++i) {
		b->bitstring[getchunk(i)] |= (1 << getbitpos(i));
	}
}

// set the bit at position to 0

void unsetBit(Bits b, int position)
{
	// TODO
	int chunk = getchunk(position), pos = getbitpos(position);
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	if (!bitIsSet(b, position)) return;
	b->bitstring[chunk] ^= (1 << pos);
}

// set all bits to 0

void unsetAllBits(Bits b)
{
	int i;
	assert(b != NULL);
	//TODO
	for (i = 0 ; i < b->nbits; ++i) {
		b->bitstring[getchunk(i)] |= (1 << getbitpos(i));
		b->bitstring[getchunk(i)] ^= (1 << getbitpos(i));
	}
}

// bitwise AND ... b1 = b1 & b2

void andBits(Bits b1, Bits b2)
{
	// TODO
	int i;
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	for (i = 0 ; i < b1->nbytes; ++i) {
		b1->bitstring[i] = b1->bitstring[i] & b2->bitstring[i];
	}
}

// bitwise OR ... b1 = b1 | b2

void orBits(Bits b1, Bits b2)
{
	// TODO
	int i;
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	for (i = 0 ; i < b1->nbytes; ++i) {
		b1->bitstring[i] = b1->bitstring[i] | b2->bitstring[i];
	}
}


void shiftBits(Bits b, int n)
{
	int i;
    assert(b != NULL);
	if (n == 0) return;
	if (n > 0) {
		for (i = b->nbits - 1; i >= 0; --i) {
			if (i < n) unsetBit(b, i);
			if (i >= n) {
				if (bitIsSet(b, i - n)) {
					setBit(b, i);
				} else {
					unsetBit(b, i);
				}
			}
		}
	} else {
		for (i = 0; i < b->nbits; ++i) {
			if (i + n < b->nbits) {
				if (bitIsSet(b, i + n)) {
					setBit(b, i);
				} else {
					unsetBit(b, i);
				}
			} else {
				unsetBit(b, i);
			}
		}
	}
}


// get a bit-string (of length b->nbytes)
// from specified position in Page buffer
// and place it in a BitsRep structure

void getBits(Page p, Offset pos, Bits b)
{
	//TODO
	//don't know if we need to assert
	memcpy(b->bitstring, addrInPage(p, pos, sizeof(Byte) * b->nbytes), b->nbytes);
}

// copy the bit-string array in a BitsRep
// structure to specified position in Page buffer

void putBits(Page p, Offset pos, Bits b)
{
	//TODO
	// again don't know if we need to throw assertion error
	Byte *ret = addrInPage(p, pos, b->nbytes);
	memcpy(ret, b->bitstring, b->nbytes);
}

// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b)
{
	assert(b != NULL);
    //printf("(%d,%d)",b->nbits,b->nbytes);
	for (int i = b->nbytes-1; i >= 0; i--) {
		for (int j = 7; j >= 0; j--) {
			Byte mask = (1 << j);
			if (b->bitstring[i] & mask)
				putchar('1');
			else
				putchar('0');
		}
	}
}
