#include "sc_path.h"
#include "sc_utils.h"
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
#   pragma comment(lib, "shlwapi")
#   include "shlobj.h"
#   include "shlwapi.h"
#elif SC_PLATFORM == SC_PLATFORM_OSX
#   include <CoreFoundation/CoreFoundation.h>
#elif SC_PLATFORM == SC_PLATFORM_LINUX
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
