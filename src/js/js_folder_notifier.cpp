#include "js_folder_notifier.h"

#include "content/dir.h"
#include "content/vfs.h"
#include "core/log.h"
#include "core/svector.h"
#include "core/bstring.h"
#include "js/js.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <SDL.h>
#include <SDL_thread.h>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#else
#define MAX_PATH 260
#endif

namespace fs = std::filesystem;

struct FileInfo {
    vfs::path path;
    int hashtime;
};

struct notifier_data_t {
    svector<FileInfo, 256> files;
    vfs::path dir;
    SDL_Thread *thread;
    int finished;
};

notifier_data_t g_script_notifier;

#ifdef _WIN32

int js_vm_notifier_watch_directory(const char *lpDir) {
    DWORD dwWaitStatus;
    HANDLE dwChangeHandles[2];

    // Watch the directory for file creation and deletion.
    dwChangeHandles[0] = FindFirstChangeNotification(
                             lpDir,                         // directory to watch
                             FALSE,                         // do not watch subtree
                             FILE_NOTIFY_CHANGE_SIZE); // watch file modify

    if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) {
        logs::info("FindFirstChangeNotification function failed er=%x", GetLastError());
        return 0;
    }

    // Watch the subtree for directory creation and deletion.
    dwChangeHandles[1] = FindFirstChangeNotification(
                             lpDir,                         // directory to watch
                             FALSE,                          // watch the subtree
                             FILE_NOTIFY_CHANGE_LAST_WRITE);  // watch file size change

    if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) {
        logs::info("FindFirstChangeNotification function failed er=%x", GetLastError());
        return 0;
    }


    // Make a final validation check on our handles.
    if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL)) {
        logs::info("Unexpected NULL from FindFirstChangeNotification er=%x", GetLastError());
        return 0;
    }

    // Change notification is set. Now wait on both notification
    // handles and refresh accordingly.
    while (1) {
        // Wait for notification.

        logs::info("Waiting for notification...", 0, 0);

        dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles, FALSE, INFINITE);

        switch (dwWaitStatus) {
            case WAIT_OBJECT_0:

                // A file was created, renamed, or deleted in the directory.
                // Refresh this directory and restart the notification.

                if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE ) {
                    logs::info("FindNextChangeNotification function failed er=%x", GetLastError());
                    return 0;
                }
                return 2;
                break;

            case WAIT_OBJECT_0 + 1:

                // A directory was created, renamed, or deleted.
                // Refresh the tree and restart the notification.

                if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE ) {
                    logs::info("FindNextChangeNotification function failed er=%x", GetLastError());
                    return 0;
                }
                return 3;
                break;

            case WAIT_TIMEOUT:

                // A timeout occurred, this would happen if some value other
                // than INFINITE is used in the Wait call and no changes occur.
                // In a single-threaded environment you might not want an
                // INFINITE wait.

                logs::info("No changes in the timeout period.");
                break;

            default:
                logs::info("Unhandled dwWaitStatus.");
                return 0;
        }
    }

    return 1;
}

#elif defined(__linux__) || defined(__android__)
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
int js_vm_notifier_watch_directory(const char *lpDir)
{
    int length, i = 0;
    int fd;
    int wd;
    char buffer[BUF_LEN];

    fd = inotify_init();

    if ( fd < 0 ) {
        logs::info( "WARNING !!! Cant init inotify for %s", lpDir);
        return 0;
    }

    wd = inotify_add_watch( fd, lpDir, IN_MODIFY );
    length = read( fd, buffer, BUF_LEN );

    if ( length < 0 ) {
        logs::info( "WARNING !!! Cant read inotify event %s", lpDir);
        return 0;
    }

    int result = 1;
    while ( i < length ) {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        if ( event->len ) {
            if ( event->mask & IN_MODIFY ) {
                if ( event->mask & IN_ISDIR ) {
                    logs::info( "WARNING !!! The directory was modified %s", event->name);
                } else {
                    logs::info( "WARNING !!! The file was modified %s", event->name);
                    result = 2;
                    break;
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }

    inotify_rm_watch( fd, wd );
    close( fd );

    return result;
}
#else
int js_vm_notifier_watch_directory(const char *lpDir)
{
    return 1;
}
#endif

static int get_time_modified(const char *path, struct tm *ftime) {
    struct tm *footime;
#ifndef _WIN32
    struct stat attrib;
    if (stat(path, &attrib) != 0)
        return 1;
#else
    struct _stat64i32 attrib;
    if (_stat(path, &attrib) != 0) {
        return 1;
    }
#endif
    footime = gmtime((const time_t *) & (attrib.st_mtime));
    memcpy(ftime, footime, sizeof(time_t));

    return 0;
}

void js_vm_notifier_create_snapshot(const char *folder) {
    struct tm ftime;
    for (auto &p: g_script_notifier.files) {
        p.path.clear();
    }

    svector<vfs::path, 256> js_files;
    for (const auto &entry : fs::directory_iterator(folder)) {
        if (entry.path().extension().string() == ".js") {
            js_files.push_back(entry.path().string().c_str());
        }
    }

    for (auto &js_path: js_files) {
        //vfs::path abspath = js_vm_get_absolute_path(js_path);
        get_time_modified(js_path, &ftime);

        int hashtime = ftime.tm_hour * 1000 + ftime.tm_min * 100 + ftime.tm_sec;
        g_script_notifier.files.push_back({js_path, hashtime});
    }
}

void js_vm_notifier_check_snapshot(void) {
    const char *js_path;
    vfs::path abspath, filepath;
    struct tm ftime;

    for (auto &note: g_script_notifier.files) {
        js_path = note.path;
        if (!*js_path) {
            return;
        }

        vfs::path abspath = js_vm_get_absolute_path(js_path);
        get_time_modified(abspath, &ftime);

        unsigned int newTime = ftime.tm_hour * 1000 + ftime.tm_min * 100 + ftime.tm_sec;
        unsigned int oldTime = note.hashtime;
        if( newTime != oldTime ) {
            note.hashtime = newTime;
            filepath.printf(":%s", js_path);
            js_vm_reload_file(filepath);
        }
    }
}

static int js_vm_notifier_watch_directory_thread(void *ptr) {
    int result;
    while (!g_script_notifier.finished) {
        result = js_vm_notifier_watch_directory( g_script_notifier.dir );
        switch( result ) {
            case 0:
                g_script_notifier.finished = 1;
                break;

            case 3:
            case 2:
                js_vm_notifier_check_snapshot();
                g_script_notifier.finished = 0;
                break;

            default :
                g_script_notifier.finished = 0;
                break;
        }
        SDL_Delay(500);
    }

    return 0;
}

void js_vm_notifier_watch_directory_init(const char *dir) {
    logs::info("start wtaching dir %s", dir);
    g_script_notifier.dir = dir;
    js_vm_notifier_create_snapshot(dir);

    g_script_notifier.thread = SDL_CreateThread(js_vm_notifier_watch_directory_thread, "watch_directory_thread", 0);
}
