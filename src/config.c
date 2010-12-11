#include "sc_config.h"
#include "sc_inifile.h"
#include "sc_path.h"

struct _sc_config sc_config;

#define GET(Ini, Type, Key, Default) \
    ((!Ini) ? (Default) : sc_inifile_get_##Type(Ini, Key, Default))

int
sc_load_config(void)
{
    int rv = 1;
    char *filename = sc_path_join(sc_get_settings_path(), "game.ini");
    sc_inifile_t *ini = sc_inifile_from_filename(filename);
    if (!ini) {
        if (sc_get_errno() != SC_ENOENT)
            sc_error_make_critical();
        rv = 0;
    }
    sc_free(filename);

    sc_config.screen_width = GET(ini, int, "video.width", 800);
    sc_config.screen_height = GET(ini, int, "video.height", 600);
    sc_config.fullscreen = GET(ini, bool, "video.fullscreen", 0);
    sc_config.fov = GET(ini, int, "video.fov", 55);
    sc_config.ansitropic_filtering = GET(ini, int, "video.ansitropic", 4);

    sc_config.dirty = 0;
    sc_free_inifile(ini);
    return rv;
}

int
sc_save_config(void)
{
    int rv;
    char *filename;
    sc_inifile_t *ini = sc_new_inifile();

    sc_inifile_set_int(ini, "video.width", sc_config.screen_width);
    sc_inifile_set_int(ini, "video.height", sc_config.screen_height);
    sc_inifile_set_bool(ini, "video.fullscreen", sc_config.fullscreen);
    sc_inifile_set_int(ini, "video.fov", sc_config.fov);
    sc_inifile_set_int(ini, "video.ansitropic",
                       sc_config.ansitropic_filtering);

    sc_path_makedirs(sc_get_settings_path());
    filename = sc_path_join(sc_get_settings_path(), "game.ini");
    rv = sc_inifile_save(ini, filename);
    sc_free(filename);
    sc_config.dirty = 0;
    return rv;
}
