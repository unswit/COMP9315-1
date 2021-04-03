// reln.c ... functions on Relations
// part of signature indexed files
// Written by John Shepherd, March 2019

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "psig.h"
#include "bits.h"
#include "hash.h"

// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char *name, char *suffix)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.%s",name,suffix);
	File f = open(fname,O_RDWR|O_CREAT,0644);
	assert(f >= 0);
	return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char *name, Count nattrs, float pF, char sigtype,
                   Count tk, Count tm, Count pm, Count bm)
{
	int i;
	Reln r = malloc(sizeof(RelnRep));
	RelnParams *p = &(r->params);
	assert(r != NULL);
	p->nattrs = nattrs;
	p->pF = pF,
	p->sigtype = sigtype;
	p->tupsize = 28 + 7*(nattrs-2);
	Count available = (PAGESIZE-sizeof(Count));
	p->tupPP = available/p->tupsize;
	p->tk = tk; 
	if (tm%8 > 0) tm += 8-(tm%8); // round up to byte size
	p->tm = tm; p->tsigSize = tm/8; p->tsigPP = available/(tm/8);
	if (pm%8 > 0) pm += 8-(pm%8); // round up to byte size
	p->pm = pm; p->psigSize = pm/8; p->psigPP = available/(pm/8);
	if (p->psigPP < 2) { free(r); return -1; }
	if (bm%8 > 0) bm += 8-(bm%8); // round up to byte size
	p->bm = bm; p->bsigSize = bm/8; p->bsigPP = available/(bm/8);
	if (p->bsigPP < 2) { free(r); return -1; }
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	addPage(r->dataf); p->npages = 1; p->ntups = 0;
	addPage(r->tsigf); p->tsigNpages = 1; p->ntsigs = 0;
	addPage(r->psigf); p->psigNpages = 1; p->npsigs = 0;
	addPage(r->bsigf); p->bsigNpages = 1; p->nbsigs = 0; // replace this
	// Create a file containing "pm" all-zeroes bit-strings,
    // each of which has length "bm" bits
	//TODO
	for (i = 0 ; i < psigBits(r); ++i) {
		// time to add a new page
		Bits curr = newBits(bsigBits(r));
		unsetAllBits(curr);
		if (i % maxBsigsPP(r) == 0 && i != 0) {
			addPage(r->bsigf);
            p->bsigNpages++;
		}
		int pid = i / maxBsigsPP(r), offset = i % maxBsigsPP(r);
		Page pp = getPage(r->bsigf, pid);
		putBits(pp, offset, curr);
		addOneItem(pp);
		putPage(r->bsigf, pid, pp);
		p->nbsigs++;
	}
	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	File f = open(fname,O_RDONLY);
	if (f < 0)
		return FALSE;
	else {
		close(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name)
{
	Reln r = malloc(sizeof(RelnRep));
	assert(r != NULL);
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	read(r->infof, &(r->params), sizeof(RelnParams));
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r)
{
	// make sure updated global data is put in info file
	lseek(r->infof, 0, SEEK_SET);
	int n = write(r->infof, &(r->params), sizeof(RelnParams));
	assert(n == sizeof(RelnParams));
	close(r->infof); close(r->dataf);
	close(r->tsigf); close(r->psigf); close(r->bsigf);
	free(r);
}

// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t)
{
    // printf("enter\n");
	assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
	Page p;  PageID pid;
	RelnParams *rp = &(r->params);
	int adddatapage = 0;
	// add tuple to last page
	pid = rp->npages-1;
	p = getPage(r->dataf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tupPP) {
		addPage(r->dataf);
		rp->npages++;
		pid++;
		adddatapage = 1;
		free(p);
		p = newPage();
		if (p == NULL) return NO_PAGE;
	}
	addTupleToPage(r, p, t);
	rp->ntups++;  //written to disk in closeRelation()
	putPage(r->dataf, pid, p);
	// compute tuple signature and add to tsigf
	//TODO
	
	PageID sigid = rp->tsigNpages - 1;
    Page tsigp = getPage(r->tsigf, sigid);
	Bits ret = makeTupleSig(r, t);
	// check if room on the last page, if not add new page
	if (pageNitems(tsigp) == rp->tsigPP) {
		addPage(r->tsigf);
		rp->tsigNpages++;
		sigid++;
		free(tsigp);
		tsigp = newPage();
		if (tsigp == NULL) return NO_PAGE;
	}
	
	putBits(tsigp, pageNitems(tsigp), ret);
	addOneItem(tsigp);
	rp->ntsigs++;
	putPage(r->tsigf, sigid, tsigp);
	// compute page signature and add to psigf
	//TODO
    //printf("here\n");
	Bits retp = makePageSig(r, t);
	// we need to add a new entry
	//printf("%d\n", nPsigs(r));
    if (adddatapage == 1 || nPsigs(r) == 0) {
        //printf("bad case\n");
		Page lst = getPage(r->psigf, nPsigPages(r) - 1);
        // if we need to add a new page signature page
		if (maxPsigsPP(r) == pageNitems(lst)) {
            addPage(r->psigf);
			rp->psigNpages++;
			free(lst);
			lst = newPage();
			if (lst == NULL) return NO_PAGE;
		}
		// we increment the number of page signatures by 1
		rp->npsigs++;
		putBits(lst, pageNitems(lst), retp);
		addOneItem(lst);
		putPage(r->psigf, rp->psigNpages - 1, lst);
	} else {
		// we need to grab the last entry of the page signature
		// and merge the bitmask with retp
		//printf("second case %d\n", nPsigPages(r) - 1);
		Page lst = getPage(r->psigf, nPsigPages(r) - 1);
		//printf("1\n");
		Bits curr = newBits(rp->pm);
        //printf("2\n");
        getBits(lst, pageNitems(lst) - 1, curr);
        //printf("3\n");
        orBits(curr, retp);
		//printf("4\n");
		putBits(lst, pageNitems(lst) - 1, curr);
		//printf("5 %d\n", rp->psigNpages - 1);
		//showBits(curr);
		putPage(r->psigf, rp->psigNpages - 1, lst);
		//printf("6\n");
		freeBits(retp);
	}
    // printf("success\n");
	// use page signature to update bit-slices
	//TODO
    
	// at this point this variable is updated correctly
    int i;
    Page bsigp = getPage(r->psigf, nPsigPages(r) - 1);
	int lstid = pageNitems(bsigp) - 1;
	// get the page signature for the last page
	Bits bp = newBits(psigBits(r));
	getBits(bsigp, lstid, bp);
	// printf("%d\n", psigBits(r));
	for (i = 0 ; i < psigBits(r); ++i) {
		//printf("try to get page offset = %d max= %d tol item in page= %d\n", offset, maxBsigsPP(r), pageNitems(currp));
		//printf("success\n");
		if (bitIsSet(bp, i)) {
		    int bpid = i / maxBsigsPP(r), offset = i % maxBsigsPP(r);
		    Bits curr = newBits(bsigBits(r));
		    Page currp = getPage(r->bsigf, bpid);
		    getBits(currp, offset, curr);
			setBit(curr, nPsigs(r) - 1);
			putBits(currp, offset, curr);
			putPage(r->bsigf, bpid, currp);
		}
		//printf("finish\n");
	}
	// printf("finish\n");
	return nPages(r)-1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r)
{
	RelnParams *p = &(r->params);
	printf("Global Info:\n");
	printf("Dynamic:\n");
    printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
    printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
	printf("Static:\n");
    printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
			p->nattrs, p->tupsize, p->tupPP);
	printf("  sigs  %s",
            p->sigtype == 'c' ? "catc" : "simc");
    if (p->sigtype == 's')
	    printf("  bits/attr: %d", p->tk);
    printf("\n");
	printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->tm, p->tsigSize, p->tsigPP);
	printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->pm, p->psigSize, p->psigPP);
	printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->bm, p->bsigSize, p->bsigPP);
}
