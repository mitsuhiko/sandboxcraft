#include "sc_path.h"
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
#   pragma comment(lib, "shlwapi")
#   include "shlobj.h"
#   include "shlwapi.h"
#elif SC_PLATFORM == SC_PLATFORM_OSX
#   include <CoreFoundation/CoreFoundation.h>
#elif SC_PLATFORM == SC_PLATFORM_LINUX
#   include <unistd.h>
#endif

const char *sc_get_resource_path(void)
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

char *sc_path_to_resource(const char *kind, const char *resource)
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
