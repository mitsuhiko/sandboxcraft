#include "sc_debug.h"
#include "sc_vec3.h"

#define TYPE_LINE       1

struct debug_object;
#define COMMON_HEADER int type; struct debug_object *next
struct debug_object {
    COMMON_HEADER;
};

struct debug_object_LINE {
    COMMON_HEADER;
    sc_vec3_t start;
    sc_vec3_t end;
};

static struct debug_object *first_obj;
static struct debug_object *last_obj;


static void *
make_debug_object(size_t size, int type)
{
    struct debug_object *rv = sc_xmalloc(size);
    rv->type = type;
    rv->next = NULL;
    if (last_obj)
        last_obj->next = rv;
    last_obj = rv;
    if (!first_obj)
        first_obj = rv;
    return rv;
}
#define ADD_DEBUG_OBJ(T) \
    make_debug_object(sizeof(struct debug_object_##T), TYPE_##T)

void
_sc_debug_add_line(const sc_vec3_t *start, const sc_vec3_t *end)
{
    struct debug_object_LINE *obj = ADD_DEBUG_OBJ(LINE);
    obj->start = *start;
    obj->end = *end;
}

void
_sc_debug_add_line_with_direction(const sc_vec3_t *start,
                                  const sc_vec3_t *direction,
                                  float length)
{
    sc_vec3_t end;
    sc_vec3_normalize(&end, direction);
    sc_vec3_mul(&end, &end, length);
    sc_vec3_add(&end, start, &end);
    _sc_debug_add_line(start, &end);
}

static void
draw_line(struct debug_object_LINE *line)
{
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(line->start.x, line->start.y, line->start.z);
        glVertex3f(line->end.x, line->end.y, line->end.z);
        glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
}

void
sc_debug_flush(void)
{
    struct debug_object *node;
    glDisable(GL_DEPTH_TEST);
    for (node = first_obj; node; node = node->next) 
        switch (node->type) {
        case TYPE_LINE:
            draw_line((void *)node);
            break;
        }
    glEnable(GL_DEPTH_TEST);
    sc_debug_clear();
}

void
sc_debug_clear(void)
{
    struct debug_object *node;
    for (node = first_obj; node; node = node->next) 
        sc_free(node);
    first_obj = NULL;
    last_obj = NULL;
}
