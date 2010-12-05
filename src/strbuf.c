#include "sc_strbuf.h"
#include "sc_utils.h"

#define INITIAL_SIZE 1024
#define GROWTH_RATE 1.4f

struct _sc_strbuf {
    char *buf;
    size_t size;
    size_t allocated;
    int released;
};

sc_strbuf_t *
sc_new_strbuf(void)
{
    sc_strbuf_t *rv = sc_xalloc(sc_strbuf_t);
    rv->buf = sc_xmalloc(INITIAL_SIZE);
    rv->size = 0;
    rv->allocated = INITIAL_SIZE;
    rv->released = 0;
    return rv;
}

void
sc_free_strbuf(sc_strbuf_t *strbuf)
{
    if (!strbuf)
        return;
    if (!strbuf->released)
        sc_free(strbuf->buf);
    sc_free(strbuf);
}

void
sc_strbuf_append(sc_strbuf_t *strbuf, const char *str)
{
    sc_strbuf_append_bytes(strbuf, str, strlen(str));
}

void
sc_strbuf_appendf(sc_strbuf_t *strbuf, const char *fmt, ...)
{
    char *buf;
    va_list argptr;
    va_start(argptr, fmt);
    buf = sc_vasprintf(argptr, fmt);
    va_end(argptr);
    sc_strbuf_append(strbuf, buf);
    sc_free(buf);
}

void
sc_strbuf_append_bytes(sc_strbuf_t *strbuf, const char *bytes, size_t cnt)
{
    while (strbuf->size + cnt < strbuf->allocated) {
        strbuf->allocated = (size_t)(strbuf->allocated * GROWTH_RATE);
        strbuf->buf = sc_xrealloc(strbuf->buf, strbuf->allocated);
    }
    memcpy(strbuf->buf + strbuf->size, bytes, cnt);
    strbuf->size += cnt;
    strbuf->buf[strbuf->size] = 0;
    strbuf->released = 0;
}

size_t
sc_strbuf_size(const sc_strbuf_t *strbuf)
{
    return strbuf->size;
}

char *
sc_strbuf_get_contents(sc_strbuf_t *strbuf, size_t *size_out)
{
    strbuf->released = 1;
    strbuf->allocated = strbuf->size + 1;
    strbuf->buf = sc_xrealloc(strbuf->buf, strbuf->allocated);
    if (size_out)
        *size_out = strbuf->size;
    return strbuf->buf;
}

char *
sc_free_strbuf_and_get_contents(sc_strbuf_t *strbuf, size_t *size_out)
{
    char *rv = sc_strbuf_get_contents(strbuf, size_out);
    sc_free_strbuf(strbuf);
    return rv;
}
