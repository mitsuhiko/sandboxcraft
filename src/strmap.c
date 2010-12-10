#include "sc_strmap.h"
#include "sc_utils.h"

/* has to be compatible with sc_strmap_pair_t */
struct node;
struct node {
    char *key;
    void *value;
    struct node *left;
    struct node *right;
};

struct _sc_strmap {
    struct node *root;
    size_t size;
};

static void
free_node(struct node *node)
{
    if (!node)
        return;
    sc_free(node->key);
    sc_free(node);
}

static void
recursive_free(struct node *node)
{
    if (!node)
        return;
    recursive_free(node->left);
    recursive_free(node->right);
    sc_free(node->key);
    sc_free(node);
}

static void
delete_node(struct node **node)
{
    struct node *old_node = *node;
    if ((*node)->left == NULL) {
        *node = (*node)->right;
        free_node(old_node);
    } else if ((*node)->right == NULL) {
        *node = (*node)->left;
        free_node(old_node);
    } else {
        void *tmp;
        struct node **pred = &(*node)->left;
        while ((*pred)->right)
            pred = &(*pred)->right;
        tmp = (*pred)->value;
        (*pred)->value = (*node)->value;
        (*node)->value = tmp;
        delete_node(pred);
    }
}


sc_strmap_t *
sc_new_strmap(void)
{
    sc_strmap_t *rv = sc_xalloc(sc_strmap_t);
    rv->size = 0;
    rv->root = NULL;
    return rv;
}

void
sc_free_strmap(sc_strmap_t *strmap)
{
    if (!strmap)
        return;
    recursive_free(strmap->root);
    sc_free(strmap);
}

void *
sc_strmap_get(const sc_strmap_t *strmap, const char *key)
{
    struct node *node = strmap->root;
    while (node) {
        int cmp = strcmp(node->key, key);
        if (cmp == 0)
            return node->value;
        node = cmp > 0 ? node->right : node->left;
    }
    return NULL;
}

void
sc_strmap_set(sc_strmap_t *strmap, const char *key, void *value)
{
    int cmp;
    struct node *node, **slot = &strmap->root;
    while (1) {
        node = *slot;
        if (!node) {
            strmap->size++;
            *slot = node = sc_xalloc(struct node);
            node->key = sc_strdup(key);
            node->value = value;
            node->left = NULL;
            node->right = NULL;
            return;
        }
        cmp = strcmp(node->key, key);
        if (cmp == 0) {
            node->value = value;
            return;
        }
        slot = cmp > 0 ? &node->right : &node->left;
    }
}

int
sc_strmap_remove(sc_strmap_t *strmap, const char *key)
{
    struct node **node = &strmap->root;
    while (*node) {
        int cmp = strcmp((*node)->key, key);
        if (cmp == 0) {
            delete_node(node);
            strmap->size--;
            return 1;
        }
        node = cmp > 0 ? &(*node)->right : &(*node)->left;
    }
    return 0;
}

size_t
sc_strmap_size(const sc_strmap_t *strmap)
{
    return strmap->size;
}

static void
each_node(struct node *node, sc_strmap_callback cb, void *closure)
{
    if (!node)
        return;
    each_node(node->left, cb, closure);
    cb(node->key, node->value, closure);
    each_node(node->right, cb, closure);
}

void
sc_strmap_each(const sc_strmap_t *strmap, sc_strmap_callback cb, void *closure)
{
    each_node(strmap->root, cb, closure);
}
