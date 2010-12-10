#include "sc_inifile.h"
#include "sc_strmap.h"
#include "sc_utils.h"

#define LINE_SIZE (1<<14)

struct _sc_inifile {
    sc_strmap_t *values;
};

struct writer_info {
    FILE *file;
    char *last_section;
    int first_section;
};

static void
clear_item(const char *key, void *value, void *closure)
{
    sc_free(value);
}

static char *
skip_to(const char *str, char c)
{
    while (*str && *str != c)
        str++;
    return (char *)str;
}

static void
write_item(const char *key, void *value, void *closure)
{
    struct writer_info *info = closure;
    const char *helper = skip_to(key, '.');
    char *section = sc_xmalloc(helper - key + 1);
    memcpy(section, key, helper - key);
    section[helper - key] = '\x00';

    if (!info->last_section || strcmp(section, info->last_section)) {
        info->last_section = section;
        if (info->first_section)
            info->first_section = 0;
        else
            fprintf(info->file, "\n");
        fprintf(info->file, "[%s]\n", section);
    } else {
        sc_free(section);
    }
    fprintf(info->file, "%s = %s\n", helper + 1, (char *)value);
}

static void
parse_file(FILE *file, sc_inifile_t *inifile)
{
    size_t length;
    char line_read[LINE_SIZE], *line, *key, *value, *section = NULL;

    while (fgets(line_read, LINE_SIZE, file)) {
        line = sc_strip_string(line_read);
        length = strlen(line);

        /* comment or empty line */
        if (length == 0 || line[0] == ';' || line[0] == '#')
            continue;

        /* new section */
        if (line[0] == '[' && line[length - 1] == ']') {
            sc_free(section);
            line[length - 1] = 0;
            section = sc_strdup(line + 1);
            continue;
        }

        /* parse the key */
        key = line;
        line = skip_to(line, '=');
        if (!*line)
            continue;
        *line = '\0';
        key = sc_strip_string(key);

        /* value is rest of line */
        value = sc_strip_string(line + 1);
        key = sc_sprintf("%s.%s", section ? section : "", key);
        sc_strmap_set(inifile->values, key, sc_strdup(value));
        sc_free(key);
    }
}

sc_inifile_t *
sc_new_inifile(void)
{
    sc_inifile_t *rv = sc_xalloc(sc_inifile_t);
    rv->values = sc_new_strmap();
    return rv;
}

void
sc_free_inifile(sc_inifile_t *inifile)
{
    if (!inifile)
        return;
    sc_strmap_each(inifile->values, clear_item, NULL);
    sc_free_strmap(inifile->values);
}

sc_inifile_t *
sc_inifile_from_filename(const char *filename)
{
    sc_inifile_t *rv;
    FILE *file = fopen(filename, "r");
    if (!file) {
        sc_set_error(SC_ENOENT, filename, 0, "Could not load ini file");
        return NULL;
    }

    rv = sc_new_inifile();
    parse_file(file, rv);
    fclose(file);
    return rv;
}

int
sc_inifile_save(const sc_inifile_t *inifile, const char *filename)
{
    struct writer_info info;
    FILE *file = fopen(filename, "w");
    if (!file) {
        sc_set_error(SC_EIO, filename, 0, "Unable to write ini to file");
        return 0;
    }

    info.file = file;
    info.last_section = NULL;
    info.first_section = 1;
    sc_strmap_each(inifile->values, write_item, &info);
    fclose(file);
    sc_free(info.last_section);

    return 1;
}

int
sc_inifile_get_int(const sc_inifile_t *inifile, const char *key, int def)
{
    int intval;
    const char *value = sc_strmap_get(inifile->values, key);
    char *endptr;
    if (value == NULL)
        return def;
    intval = strtol(value, &endptr, 10);
    return (endptr == value) ? def : intval;
}

void
sc_inifile_set_int(const sc_inifile_t *inifile, const char *key, int val)
{
    void *old_val = sc_strmap_get(inifile->values, key);
    char *value;
    assert(sc_inifile_key_is_valid(key));
    sc_free(old_val);
    value = sc_sprintf("%d", val);
    sc_strmap_set(inifile->values, key, value);
}

int
sc_inifile_get_bool(const sc_inifile_t *inifile, const char *key, int def)
{
    const char *rv = sc_strmap_get(inifile->values, key);
    if (!rv)
        return def;
    if (strcmp(rv, "true") == 0 || strcmp(rv, "1") == 0)
        return 1;
    if (strcmp(rv, "false") == 0 || strcmp(rv, "0") == 0)
        return 0;
    return def;
}

void
sc_inifile_set_bool(const sc_inifile_t *inifile, const char *key, int val)
{
    void *old_val = sc_strmap_get(inifile->values, key);
    char *value;
    assert(sc_inifile_key_is_valid(key));
    sc_free(old_val);
    sc_strmap_set(inifile->values, key, sc_strdup(value ? "true" : "false"));
}

float
sc_inifile_get_float(const sc_inifile_t *inifile, const char *key, float def)
{
    float floatval;
    const char *value = sc_strmap_get(inifile->values, key);
    char *endptr;
    if (value == NULL)
        return def;
    floatval = strtof(value, &endptr);
    return (endptr == value) ? def : floatval;
}

void
sc_inifile_set_float(const sc_inifile_t *inifile, const char *key, float val)
{
    void *old_val = sc_strmap_get(inifile->values, key);
    char *value;
    assert(sc_inifile_key_is_valid(key));
    sc_free(old_val);
    value = sc_sprintf("%f", val);
    sc_strmap_set(inifile->values, key, value);
}

const char *
sc_inifile_get_string(const sc_inifile_t *inifile, const char *key,
                      const char *def)
{
    const char *rv = sc_strmap_get(inifile->values, key);
    return rv ? rv : def;
}

void
sc_inifile_set_string(const sc_inifile_t *inifile, const char *key,
                      const char *val)
{
    void *old_val = sc_strmap_get(inifile->values, key);
    assert(sc_inifile_key_is_valid(key));
    sc_free(old_val);
    sc_strmap_set(inifile->values, key, sc_strdup(val));
}

void
sc_inifile_remove_option(const sc_inifile_t *inifile, const char *key)
{
    void *old_val = sc_strmap_get(inifile->values, key);
    assert(sc_inifile_key_is_valid(key));
    sc_strmap_remove(inifile->values, key);
    sc_free(old_val);
}

int
sc_inifile_key_is_valid(const char *key)
{
    while (*key)
        if (*key++ == '.')
            return 1;
    return 0;
}
