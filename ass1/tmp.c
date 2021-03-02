/*
 * src/tutorial/complex.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/
/*
#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		

PG_MODULE_MAGIC;
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
const int MY_INT_MAX = 2147483647;
////////// ADT session //////////////////////////////////
struct intSet {
    int sz, capacity;
    int *list;
};

struct stringBuf {
    int sz, capacity;
    char *text;
};

typedef struct intSet *intSet;
typedef struct stringBuf *mystring;

static mystring newBuf() {
    mystring ret = malloc(sizeof(struct stringBuf));
    ret->capacity = 24;
    ret->sz = 0;
    ret->text = malloc(24 * sizeof(char));
    return ret;
}

static void append(mystring s, char *st) {
    int i, len;
    if (st == NULL) return;
    len = strlen(st);
    if (len == 0) return;
    while (s->sz + 14 + len > s->capacity) {
        s->capacity *= 2;
        s->text = realloc(s->text, s->capacity * sizeof(char));
    }

    for (i = 0 ; i < len; ++i) {
        s->text[s->sz++] = st[i];
    }
    s->text[s->sz] = '\0';
    // assert(strlen(s->text) == s->sz);
}

static intSet newset() {
    intSet ret = malloc(sizeof(struct intSet));
    ret->sz = 0;
    ret->capacity = 16;
    ret->list = malloc(ret->capacity * sizeof(int));
    return ret;
}

static int my_iswhite(char ch) {
    if (ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r' || ch == ' ') return 1;
    return 0; 
}

static int my_issep(char ch) {
    return ch == '{' || ch == ',' || ch == '}';
}

static int my_isdigit(char ch) {
    return ch >= '0' && ch <= '9';
}

static int intset_size(intSet s) {
    assert(s != NULL);
    return s->sz;
}
/*
static void swap(int *a, int *b) {
    int p = *a;
    *a = *b;
    *b = p;
}
*/
static int intset_inSet(intSet s, int val) {
    int low = 0, high = s->sz - 1;
    if (s == NULL) return 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (s->list[mid] == val) return 1;
        if (s->list[mid] < val) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return 0;
}

static void intset_insertSet(intSet s, int val) {
    if (s->sz == s->capacity) {
        s->capacity = s->capacity * 2;
        s->list = realloc(s->list, s->capacity * sizeof(int));
    }
    s->list[s->sz++] = val;
}

static int cmpfunc (const void * a, const void * b) {
   return (*(int*)a - *(int*)b);
}


static void intset_normalize(intSet s) {
    int i, len;
    int *tmp;
    if (s == NULL || s->sz == 0) return;
    tmp = calloc(s->sz, sizeof(int));
    len = s->sz;
    for (i = 0 ; i < s->sz; ++i) {
        tmp[i] = s->list[i];
    }

    qsort(tmp, s->sz, sizeof(int), cmpfunc);
    free(s->list);
    s->sz = 0;
    s->capacity = 16;
    s->list = malloc(s->capacity * sizeof(int));
    for (i = 0 ; i < len; ++i) {
        if (i == 0 || tmp[i] != tmp[i-1]) intset_insertSet(s, tmp[i]);
    }
    free(tmp);
}

static void clearS(intSet s) {
    free(s->list);
    free(s);
}

static void clearStr(mystring s) {
    free(s->text);
    free(s);
}

static char *printSet(intSet s) {
    int i;
    if (s == NULL) {
        return NULL;
    } else {
        mystring res = newBuf();
        char *ret;
        char *st = calloc(100, sizeof(char));
        st[0] = '{';
        append(res, st);
        for (i = 0; i < s->sz; ++i) {
            if (i != 0) {
                append(res, ",");
            }
            sprintf(st, "%d", s->list[i]);
            append(res, st);
        }
        append(res, "}");
        ret = malloc(sizeof(char) * (res->sz + 1));
        ret[res->sz] = '\0';
        strcpy(ret, res->text);
        free(res->text);
        free(res);
        free(st);
        return ret;
    }
}

static intSet get_new(char *s) {
    int i, len, L = 0, R, c1 = 0, c2 = 0;
    char *ss = NULL;
    mystring *vc;
    int tol = 0, valid = 1;
    intSet ret;
    if (s == NULL) return NULL;
    len = strlen(s), R = len - 1;
    for (i = 0; i < len; ++i) {
        if (s[i] < '0' || s[i] > '9') {
            if (my_issep(s[i]) || my_iswhite(s[i])) continue;
            return NULL;
        }
    }

    for (i = 0 ; i < len; ++i) {
        if (s[i] == '{') {
            L = i + 1;
            break;
        }
        if (!my_iswhite(s[i])) return NULL;
    }
    
    for (i = len - 1; i >= 0; --i) {
        if (s[i] == '}') {
            R = i - 1;
            break;
        }
        if (!my_iswhite(s[i])) return NULL;
    }

    for (i = 0; i < len; ++i) {
        if (s[i] == '{') c1++;
        if (s[i] == '}') c2++;
    }
    
    if (c1 != 1 || c2 != 1) return NULL;
    ret = newset();
    vc = calloc(len + 3, sizeof(mystring));
    for (i = 0 ; i < len + 3; ++i) vc[i] = newBuf();
    for (i = L; i <= R; ++i) {
        // printf("%d\n", i);
        if (my_isdigit(s[i])) {
            // [digit] (white space) [digit] break case!
            if (vc[tol]->sz == 0 && tol > 0 && my_isdigit(vc[tol-1]->text[0])) {
                valid = 0;
                break;
            }
            ss = calloc(2, sizeof(char));
            ss[0] = s[i], ss[1] = '\0';
            append(vc[tol], ss);
            free(ss);
            // overflow case
            if (atoll(vc[tol]->text) > 1ll * MY_INT_MAX) {
                valid = 0;
                break;
            }
        } else if (my_issep(s[i])) {
            if (tol == 0 && vc[tol]->sz == 0) {
                // {, break case
                valid = 0;
                break;
            }
            
            if (my_issep(vc[tol]->text[0]) 
            || (tol > 0 && vc[tol]->sz == 0 && my_issep(vc[tol-1]->text[0]))) {
                // ,,
                valid = 0;
                break;
            }
            
            if (vc[tol]->sz > 0) {
                tol++;
            }
            ss = calloc(2, sizeof(char));
            ss[0] = s[i], ss[1] = '\0';
            append(vc[tol], ss);
            free(ss);
            tol++;
        } else {
            if (vc[tol]->sz != 0) tol++;
        }
    }
    // ,}
    for (i = tol; i >= 0; --i) {
        if (vc[i]->sz > 0) {
            if (my_issep(vc[i]->text[0])) valid = 0;
            break;
        }
    }

    if (valid) {
        for (i = 0 ; i <= tol; ++i) {
            if (vc[i]->sz == 0) continue;
            if (my_isdigit(vc[i]->text[0])) intset_insertSet(ret, atoi(vc[i]->text));
        }
    }
    for (i = 0 ; i < len + 3; ++i) {
        clearStr(vc[i]);
    }

    free(vc);
    if (!valid) {
        clearS(ret);
        return NULL;
    }
    intset_normalize(ret);
    return ret;
}

static intSet intset_unionSet(intSet s1, intSet s2) {
    int i;
    intSet res;
    if (s1 == NULL || s2 == NULL) return NULL;
    res = newset();
    for (i = 0; i < s1->sz; ++i) {
        intset_insertSet(res, s1->list[i]);
    }

    for (i = 0; i < s2->sz; ++i) {
        intset_insertSet(res, s2->list[i]);
    }

    intset_normalize(res);
    return res;
}

static intSet intset_intersectionSet(intSet s1, intSet s2) {
    int i;
    intSet res;
    if (s1 == NULL || s2 == NULL) return NULL;
    res = newset();
    for (i = 0; i < s1->sz; ++i) {
        if (intset_inSet(s2, s1->list[i])) {
            intset_insertSet(res, s1->list[i]);
        }
    }

    intset_normalize(res);
    return res;
}

static intSet intset_disjointSet(intSet s1, intSet s2) {
    int i;
    intSet res;
    if (s1 == NULL || s2 == NULL) return NULL;
    res = newset();
    for (i = 0; i < s1->sz; ++i) {
        if (!intset_inSet(s2, s1->list[i])) {
            intset_insertSet(res, s1->list[i]);
        }
    }

    for (i = 0; i < s2->sz; ++i) {
        if (!intset_inSet(s1, s2->list[i])) {
            intset_insertSet(res, s2->list[i]);
        }
    }

    intset_normalize(res);
    return res;
}

static intSet intset_differenceSet(intSet s1, intSet s2) {
    int i;
    intSet res;
    if (s1 == NULL || s2 == NULL) return NULL;
    res = newset();
    for (i = 0 ; i < s1->sz; ++i) {
        if (!intset_inSet(s2, s1->list[i])) intset_insertSet(res, s1->list[i]);
    }

    intset_normalize(res);
    return res;
}

// check if s1 <= s2
static int intset_subset(intSet s1, intSet s2) {
    int i;
    if (s1 == NULL || s2 == NULL) return -1;
    for (i = 0 ; i < s1->sz; ++i) {
        if (!intset_inSet(s2, s1->list[i])) return 0;
    }
    return 1;
}

static int intset_equal(intSet s1, intSet s2) {
    return intset_subset(s1, s2) && intset_subset(s2, s1);
}

////////////////////// the below session is used for interaction ///////////
/*
// read in
PG_FUNCTION_INFO_V1(my_intset_in);
Datum
my_intset_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    intSet ret = get_new(str);
    if (ret == NULL) {
        ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"intset", str)));
    }

    PG_RETURN_POINTER(ret);
}

// output
PG_FUNCTION_INFO_V1(my_intset_out);
Datum
my_intset_out(PG_FUNCTION_ARGS)
{
	intSet ret = (intSet) PG_GETARG_POINTER(0);
	char *result = printSet(ret);
	PG_RETURN_CSTRING(result);
}

// cardinality
PG_FUNCTION_INFO_V1(my_intset_size);
Datum
my_intset_size(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
	PG_RETURN_INT32(intset_size(a));
}


// check element in a set
PG_FUNCTION_INFO_V1(my_inset);
Datum
my_inset(PG_FUNCTION_ARGS)
{
    int i = PG_GETARG_INT32(0);
	intSet a = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(intset_inSet(a, i));
}


// check if a is a superset of b
PG_FUNCTION_INFO_V1(my_superset);
Datum
my_superset(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(intset_subset(b, a));
}

// check if a is a superset of b
PG_FUNCTION_INFO_V1(my_subset);
Datum
my_subset(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(intset_subset(a, b));
}

// check equal
PG_FUNCTION_INFO_V1(my_equal);
Datum
my_equal(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(intset_equal(a, b));
}

PG_FUNCTION_INFO_V1(my_inequal);
Datum
my_inequal(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(!intset_equal(a, b));
}

// a && b
PG_FUNCTION_INFO_V1(my_intersection);
Datum
my_intersection(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_POINTER(intset_intersectionSet(a, b));
}

// a V b
PG_FUNCTION_INFO_V1(my_union);
Datum
my_union(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_POINTER(intset_unionSet(a, b));
}

// (a - b) V (b - a)
PG_FUNCTION_INFO_V1(my_disjoin);
Datum
my_disjoin(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_POINTER(intset_disjointSet(a, b));
}

// a - b
PG_FUNCTION_INFO_V1(my_difference);
Datum
my_difference(PG_FUNCTION_ARGS)
{
	intSet  a = (intSet) PG_GETARG_POINTER(0);
    intSet  b = (intSet) PG_GETARG_POINTER(1);
	PG_RETURN_POINTER(intset_differenceSet(a, b));
}

*/

int main() {

    return 0;
}