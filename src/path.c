#include "sc_path.h"
#include "sc_strbuf.h"
#include "sc_utils.h"
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
#   pragma comment(lib, "shlwapi")
#   include "shlobj.h"
#   include "shlwapi.h"
#   define mkdir(Fn, Mode) _mkdir(Fn)
#elif SC_PLATFORM == SC_PLATFORM_OSX
#   include <CoreFoundation/CoreFoundation.h>
#endif
#if SC_PLATFORM == SC_PLATFORM_OSX || SC_PLATFORM == SC_PLATFORM_LINUX
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <pwd.h>
#   include <unistd.h>
#endif

const char *
sc_get_resource_path(void)
{
    static char *path = NULL;
    if (path != NULL)
        return path;

#if SC_PLATFORM == SC_PLATFORM_WINDOWS
    {
        char *pathptr;
        path = sc_xmalloc(MAX_PATH + 20);

        GetModuleFileNameA(0, path, MAX_PATH);
        while (1) {
            if ((pathptr = strrchr(path, '\\')) == 0) {
                /* assume working directory, shouldn't happen anyways */
                strcpy(path, ".");
                return path;
            }
            strcpy(pathptr, "\\resources");
            if (PathIsDirectoryA(path))
                break;
            *pathptr = '\0';
        }
    }
#elif SC_PLATFORM == SC_PLATFORM_LINUX
    char *dirname = get_current_dir_name();
    path = sc_xmalloc(strlen(dirname) + 30);
    strcpy(path, dirname);
    strcat(path, "/resources");
    free(dirname);
#elif SC_PLATFORM == SC_PLATFORM_OSX
    path = sc_xmalloc(4096);
    CFBundleRef main_bundle = CFBundleGetMainBundle();
    CFURLRef main_bundle_url = CFBundleCopyBundleURL(main_bundle);
    CFStringRef cf_stringref = CFURLCopyFileSystemPath(main_bundle_url,
                                                       kCFURLPOSIXPathStyle);
    CFStringGetCString(cf_stringref, path, 4000, kCFStringEncodingASCII);
    CFRelease(main_bundle_url);
    CFRelease(cf_stringref);
    strcat(path, "/Contents/Resources");
#else
#   error "Unsupported platform in sc_get_resource_path"
#endif

    return path;
}

const char *
sc_get_home_path(void)
{
    static char *path = NULL;
    if (path != NULL)
        return path;

#if SC_PLATFORM == SC_PLATFORM_WINDOWS
#elif SC_PLATFORM == SC_PLATFORM_LINUX || SC_PLATFORM == SC_PLATFORM_OSX
    const char *env_home = getenv("HOME");
    if (env_home) {
        path = sc_strdup(env_home);
    } else {
        struct passwd *pw = getpwuid(getuid());
        size_t end;
        endpwent();
        if (!pw)
            sc_critical_error(SC_EIO, __FILE__, __LINE__,
                              "Unable to find home folder");
        path = sc_strdup(pw->pw_dir);
        end = strlen(path);
        if (end && path[end - 1] == '/')
            path[end - 1] = '\x00';
    }
#else
#   error "Unsupported platform in sc_get_home_path"
#endif

    return path;
}

const char *
sc_get_settings_path(void)
{
    static char *path = NULL;
    if (path != NULL)
        return path;

#if SC_PLATFORM == SC_PLATFORM_WINDOWS
    {
        sc_strbuf_t *strbuf = sc_new_strbuf();
        char tmp[PATH_MAX], *pathptr;
        LPITEMIDLIST pidl;
        SHGetFolderLocation(0, CSIDL_APPDATA, 0, 0, &pidl);
        SHGetPathFromIDList(pidl, tmp);
        CoTaskMemFree(pidl);
        sc_strbuf_append(strbuf, path);
        sc_strbuf_append(strbuf, "\\SandboxCraft");
        path = sc_free_strbuf_and_get_contents(strbuf, NULL);
    }
#elif SC_PLATFORM == SC_PLATFORM_LINUX
    sc_strbuf_t *strbuf = sc_new_strbuf();
    const char *xdg = getenv("XDG_CONFIG_HOME");
    if (!xdg || !*xdg) {
        sc_strbuf_append(strbuf, sc_get_home_path());
        sc_strbuf_append(strbuf, "/.config/sandboxcraft");
    } else {
        sc_strbuf_append(strbuf, xdg);
        sc_strbuf_append(strbuf, "/sandboxcraft");
    }
    path = sc_free_strbuf_and_get_contents(strbuf, NULL);
#elif SC_PLATFORM == SC_PLATFORM_OSX
    sc_strbuf_t *strbuf = sc_new_strbuf();
    sc_strbuf_append(strbuf, sc_get_home_path());
    sc_strbuf_append(strbuf, "/Library/Application Support/SandboxCraft");
    path = sc_free_strbuf_and_get_contents(strbuf, NULL);
#else
#   error "Unsupported platform in sc_get_settings_path"
#endif
    return path;
}

char *
sc_path_to_resource(const char *kind, const char *resource)
{
    const char *path = sc_get_resource_path();
    size_t pathlen = strlen(path);
    size_t kindlen = strlen(kind);
    size_t resourcelen = strlen(resource);

    char *buf = sc_xmalloc(pathlen + kindlen + resourcelen + 3);
    memcpy(buf, path, pathlen);
    buf[pathlen] = '/';
    memcpy(buf + pathlen + 1, kind, kindlen);
    buf[pathlen + kindlen + 1] = '/';
    strcpy(buf + pathlen + kindlen + 2, resource);
    return buf;
}

char *
sc_path_join(const char *a, const char *b)
{
    size_t al = strlen(a);
    size_t bl = strlen(b);
    char *rv = sc_xmalloc(al + bl + 2);
    memcpy(rv, a, al);
    rv[al] = '/';
    memcpy(rv + al + 1, b, bl + 1);
    return rv;
}

char *
sc_path_dirname(const char *path)
{
    const char *last_slash = strrchr(path, '/');
    size_t length;
    char *rv;

#if SC_PLATFORM == SC_PLATFORM_WINDOWS
    const char *last_backslash = strrchr(path, '\\');
    last_slash = (char *)sc_max((uintptr_t)last_slash,
                                (uintptr_t)last_backslash);
#endif
    if (!last_slash)
        return sc_strdup(".");

    length = last_slash - path;
    rv = sc_xmalloc(length + 1);
    memcpy(rv, path, length);
    rv[length] = '\0';
    return rv;
}

char *
sc_path_join_with_dir(const char *a, const char *b)
{
    char *dirname = sc_path_dirname(a);
    char *rv = sc_path_join(dirname, b);
    sc_free(dirname);
    return rv;
}

int
sc_path_delete_file(const char *path)
{
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
    if (!DeleteFileA(path) && 
        GetLastError() != ERROR_FILE_NOT_FOUND &&
        GetLastError() != ERROR_PATH_NOT_FOUND)
        goto error;
#else
    if (unlink(path) < 0 && errno == ENOENT)
        goto error;
#endif
    return 1;

error:
    sc_set_error(SC_EIO, path, 0, "Could not delete file");
    return 0;
}

/* XXX: this is really, really ugly but it works.  Clean this up at times */
int
sc_path_makedirs(const char *path)
{
    int rv, first_try = 1;
try_again:
    if (mkdir(path, 0700) == 0)
        return 1;
    if (errno == EEXIST)
        return 1;
    else if (errno == ENOENT || errno == ENOTDIR) {
        char *parent = sc_path_dirname(path);
        if (strcmp(parent, ".") == 0 ||
            strcmp(parent, path) == 0) {
            sc_free(parent);
            goto error;
        }
        rv = sc_path_makedirs(parent);
        sc_free(parent);
        if (!rv || !first_try) {
            sc_augment_error_context(path, 0);
            return 0;
        }
        first_try = 0;
        goto try_again;
    }

error:
    sc_set_error(SC_EIO, path, __LINE__, 0, "Could not create folder");
    return 0;
}
