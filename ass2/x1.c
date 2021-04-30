// Test Bits ADT

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"

int main(int argc, char **argv)
{
	Bits b = newBits(13);
	showBits(b);
	shiftBits(b, 1);
	freeBits(b);
	return 0;
}
