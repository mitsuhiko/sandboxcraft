#include "sc_utils.h"

#include <stddef.h>

/* sort:
   J. L. Bentley and M. D. McIlroy. Engineering a sort function.
   Software---Practice and Experience, 23(11):1249-1265. */

/* that does not appear to be too reliable.  It will still work on
   64 bit but it's not the wordsize.  Do I care? */
typedef int word_t;

#define WORD_SIZE sizeof(word_t)
#define BASIC_SWAP(a, b, t) (t = a, a = b, b = t);

static void
swapfunc(char *a, char *b, size_t n, int swaptype)
{
    if (swaptype <= 1) {
        word_t t;
        for (; n > 0; a += WORD_SIZE, b += WORD_SIZE, n -= WORD_SIZE)
            BASIC_SWAP(*(word_t *)a, *(word_t *)b, t);
    }
    else {
        char t;
        for (; n > 0; a += 1, b += 1, n -= 1)
            BASIC_SWAP(*a, *b, t);
    }
}

static char *
med3(void *a, void *b, void *c, sc_cmpfunc cmp, void *closure)
{
    return cmp(a, b, closure) < 0
        ? (cmp(b, c, closure) < 0 ? b : cmp(a, c, closure) < 0 ? c : a)
        : (cmp(b, c, closure) > 0 ? b : cmp(a, c, closure) > 0 ? c : a);
}

void
sort_impl(char *a, size_t n, size_t es, void *closure, sc_cmpfunc cmp)
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn, *pv;
    int r;
    word_t t, v;
    size_t s;
 
    /* helpers for swapping */
    int swaptype = ((size_t)a | es) % WORD_SIZE ? 2 : es > WORD_SIZE ? 1 : 0;
#define SWAP(a, b) do { \
    swaptype != 0 ? swapfunc(a, b, es, swaptype) : \
    (void)BASIC_SWAP(*(word_t*)(a), *(word_t*)(b), t); \
} while (0)
 
    /* insertion sort on smallest arrays */
    if (n < 7) {
        for (pm = a + es; pm < a + n * es; pm += es)
            for (pl = pm; pl > a && cmp(pl - es, pl, closure) > 0; pl -= es)
                SWAP(pl, pl - es);
        return;
    }

    /* small arrays, middle element */
    pm = a + (n / 2) * es;
    if (n > 7) {
        pl = a;
        pn = a + (n - 1) * es;

        /* big arrays, pseudomedian of 9 */
        if (n > 40) {
            s = (n / 8) * es;
            pl = med3(pl, pl + s, pl + 2 * s, cmp, closure);
            pm = med3(pm - s, pm, pm + s, cmp, closure);
            pn = med3(pn - 2 * s, pn - s, pn, cmp, closure);
        }

        /* mid-size, median of 3 */
        pm = med3(pl, pm, pn, cmp, closure);
    }

    /* pv points to parition value */
    if (swaptype) {
        pv = a;
        SWAP(pv, pm);
    }
    else {
        pv = (char *)&v;
        v = *(word_t *)pm;
    }

    pa = pb = a;
    pc = pd = a + (n - 1) * es;
    while (1) {
        while (pb <= pc && (r = cmp(pb, pv, closure)) <= 0) {
            if (r == 0) {
                SWAP(pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pc >= pb && (r = cmp(pc, pv, closure)) >= 0) {
            if (r == 0) {
                SWAP(pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        SWAP(pb, pc);
        pb += es;
        pc -= es;
    }

    pn = a + n * es;
    s = sc_min(pa-a, pb - pa);
    if (s > 0)
        swapfunc(a, pb - s, s, swaptype);
    s = sc_min(pd - pc, pn - pd - es);
    if (s > 0)
        swapfunc(pb, pn - s, s, swaptype);
    if ((s = pb - pa) > es)
        sort_impl(a, s / es, es, closure, cmp);
    if ((s = pd - pc) > es)
        sort_impl(pn - s, s / es, es, closure, cmp);

    #undef SWAP
}

void
sc_sort(void *base, size_t length, size_t size, void *closure,
        sc_cmpfunc cmpfunc)
{
    if (length <= 1)
        return;
    sort_impl((char *)base, length, size, closure, cmpfunc);
}
