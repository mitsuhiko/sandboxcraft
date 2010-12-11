/* utilities for path manipulation */
#ifndef _INC_SC_PATH_H_
#define _INC_SC_PATH_H_

#include "sc_boot.h"

/* returns the path to the resource folder */
const char *sc_get_resource_path(void);

/* return the path to the home folder */
const char *sc_get_home_path(void);

/* returns the path to the game configuration directory.  This does
   not create the directory however. */
const char *sc_get_settings_path(void);

/* joins two paths */
char *sc_path_join(const char *a, const char *b);

/* returns the directory name of a path */
char *sc_path_dirname(const char *path);

/* joins the directory of the given path with another path */
char *sc_path_join_with_dir(const char *a, const char *b);

/* returns the path to a specific resource of a specific kind.
   The kind corresponds to the folder the resource is located in.
   If subfolders are used, slashes must be used to access a
   resource therein.  The return value must be sc_free()ed */
char *sc_path_to_resource(const char *kind, const char *resource);

/* deletes a file.  Sets SC_EIO if that is not possible and returns 0.
   If the file wasn't there in the first place or it was deleted
   properly, 1 is returned. */
int sc_path_delete_file(const char *path);

/* creates all necessary directories up to path.  If all already
   existed or it succeeded, 1 is returned.  Otherwise SC_EIO is set
   and 0 is returned. */
int sc_path_makedirs(const char *path);

#endif
