#include "js_folder_notifier.h"

#include "content/dir.h"
#include "content/vfs.h"
#include "core/log.h"
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

#ifdef _WIN32
#include <Windows.h>
#endif

struct FileInfo {
    vfs::path path;
    int hashtime;
};

static struct {
    FileInfo files[256];
    vfs::path dir;
    SDL_Thread *thread;
    int finished;
} notifier;

#ifdef _WIN32

int js_vm_notifier_watch_directory(const char *lpDir) {
    DWORD dwWaitStatus;
    HANDLE dwChangeHandles[2];
    char lpDirStorage[MAX_PATH];
    strncpy(lpDirStorage, lpDir, MAX_PATH);

    // Watch the directory for file creation and deletion.
    dwChangeHandles[0] = FindFirstChangeNotification(
                             lpDir,                         // directory to watch
                             FALSE,                         // do not watch subtree
                             FILE_NOTIFY_CHANGE_SIZE); // watch file modify

    if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) {
        logs::info("FindFirstChangeNotification function failed er=", 0, GetLastError());
        return 0;
    }

    // Watch the subtree for directory creation and deletion.
    dwChangeHandles[1] = FindFirstChangeNotification(
                             lpDirStorage,                         // directory to watch
                             FALSE,                          // watch the subtree
                             FILE_NOTIFY_CHANGE_LAST_WRITE);  // watch file size change

    if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) {
        logs::info("FindFirstChangeNotification function failed er=", 0, GetLastError());
        return 0;
    }


    // Make a final validation check on our handles.
    if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL)) {
        logs::info("Unexpected NULL from FindFirstChangeNotification er=", 0, GetLastError());
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
                    logs::info("FindNextChangeNotification function failed er=", 0, GetLastError());
                    return 0;
                }
                return 2;
                break;

            case WAIT_OBJECT_0 + 1:

                // A directory was created, renamed, or deleted.
                // Refresh the tree and restart the notification.

                if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE ) {
                    logs::info("FindNextChangeNotification function failed er=", 0, GetLastError());
                    return 0;
                }
                return 3;
                break;

            case WAIT_TIMEOUT:

                // A timeout occurred, this would happen if some value other
                // than INFINITE is used in the Wait call and no changes occur.
                // In a single-threaded environment you might not want an
                // INFINITE wait.

                logs::info("No changes in the timeout period.", 0, 0);
                break;

            default:
                logs::info("Unhandled dwWaitStatus.", 0, 0);
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
        log_info( "WARNING !!! Cant init inotify for ", lpDir, 0);
        return 0;
    }

    wd = inotify_add_watch( fd, lpDir, IN_MODIFY );
    length = read( fd, buffer, BUF_LEN );

    if ( length < 0 ) {
        log_info( "WARNING !!! Cant read inotify event ", lpDir, 0);
        return 0;
    }

    int result = 1;
    while ( i < length ) {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        if ( event->len ) {
            if ( event->mask & IN_MODIFY ) {
                if ( event->mask & IN_ISDIR ) {
                    log_info( "WARNING !!! The directory was modified", event->name, 0 );
                } else {
                    log_info( "WARNING !!! The file was modified", event->name, 0 );
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
    const char *js_path;
    struct tm ftime;
    vfs::path filepath, abspath;
    for (int i = 0; i < MAX_PATH; ++i) {
        notifier.files[i].path.clear();
    }

    const dir_listing *js_files = vfs::dir_find_files_with_extension(folder, "js");

    for (int i = 0; i < js_files->num_files; ++i) {
        js_path = js_files->files[i];

        abspath = js_vm_get_absolute_path(filepath);
        get_time_modified(abspath, &ftime);

        notifier.files[i].hashtime = ftime.tm_hour * 1000 + ftime.tm_min * 100 + ftime.tm_sec;
        strncpy(notifier.files[i].path, js_path, MAX_PATH);
    }
}

void js_vm_notifier_check_snapshot(void) {
    const char *js_path;
    vfs::path abspath, filepath;
    struct tm ftime;

    for (int i = 0; i < MAX_PATH; ++i) {
        js_path = notifier.files[i].path;
        if (!*js_path) {
            return;
        }

        vfs::path abspath = js_vm_get_absolute_path(js_path);
        get_time_modified(abspath, &ftime);

        unsigned int newTime = ftime.tm_hour * 1000 + ftime.tm_min * 100 + ftime.tm_sec;
        unsigned int oldTime = notifier.files[i].hashtime;
        if( newTime != oldTime ) {
            notifier.files[i].hashtime = newTime;
            filepath.printf(":%s", js_path);
            js_vm_reload_file(filepath);
        }
    }
}

static int js_vm_notifier_watch_directory_thread(void *ptr) {
    int result;
    while (!notifier.finished) {
        result = js_vm_notifier_watch_directory( notifier.dir );
        switch( result ) {
            case 0:
                notifier.finished = 1;
                break;

            case 3:
            case 2:
                js_vm_notifier_check_snapshot();
                notifier.finished = 0;
                break;

            default :
                notifier.finished = 0;
                break;
        }
        SDL_Delay(500);
    }

    return 0;
}

void js_vm_notifier_watch_directory_init(const char *dir) {
    logs::info("start wtaching dir", dir, 0);
    strncpy(notifier.dir, dir, MAX_PATH);
    js_vm_notifier_create_snapshot(dir);

    notifier.thread = SDL_CreateThread(js_vm_notifier_watch_directory_thread, "watch_directory_thread", 0);
}