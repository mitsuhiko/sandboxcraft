/* simple interface to ini files.  Allows reading and writing though writing
   back currently destroys added comments and whitespace. */
#ifndef _INC_SC_INIFILE_H_
#define _INC_SC_INIFILE_H_

#include "sc_boot.h"

struct _sc_inifile;
typedef struct _sc_inifile sc_inifile_t;

/* creates a new and empty ini file */
sc_inifile_t *sc_new_inifile(void);

/* frees the inifile */
void sc_free_inifile(sc_inifile_t *inifile);

/* loads an ini file from a given filename.  If that file does not exist
   an SC_ENOENT error code is set and NULL is returned. */
sc_inifile_t *sc_inifile_from_filename(const char *filename);

/* stores an inifile into a file on the filesystem.  If that did not work
   due to permission errors 0 is returned and SC_EIO is set as error. */
int sc_inifile_save(const sc_inifile_t *inifile, const char *filename);

/* returns an integer value from the ini file */
int sc_inifile_get_int(const sc_inifile_t *inifile, const char *key, int def);

/* sets an option to an integer */
void sc_inifile_set_int(const sc_inifile_t *inifile, const char *key, int val);

/* returns a boolean value from the ini file */
int sc_inifile_get_bool(const sc_inifile_t *inifile, const char *key, int def);

/* sets an option to a bool */
void sc_inifile_set_bool(const sc_inifile_t *inifile, const char *key, int val);

/* returns a float from the inifile */
float sc_inifile_get_float(const sc_inifile_t *inifile, const char *key, float def);

/* sets an option to a float */
void sc_inifile_set_float(const sc_inifile_t *inifile, const char *key, float val);

/* returns a string value from the inifile.  The string returned is a pointer
   to the internal value and subject to change.  Be sure to copy it over
   before doing changes to the inifile. */
const char *sc_inifile_get_string(const sc_inifile_t *inifile, const char *key,
                                  const char *def);

/* sets a new string key */
void sc_inifile_set_string(const sc_inifile_t *inifile, const char *key,
                           const char *val);

/* removes an option */
void sc_inifile_remove_option(const sc_inifile_t *inifile, const char *key);

#endif
