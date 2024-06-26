#include "vita.h"

#include "input/touch.h"
#include "io/file.h"

#include <vita2d.h>
#include <vitasdk.h>

#include "SDL.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

// max heap size is approx. 330 MB with -d ATTRIBUTE2=12, otherwise max is 192
int _newlib_heap_size_user = 300 * 1024 * 1024;

void platform_init_callback(void) {
    if (!vita2d_init()) {
        logs::info("Exiting: vita2d init failed");
        exit(-1);
    }

    // Black
    vita2d_set_clear_color(RGBA8(0, 0, 0, 255));

    touch_set_mode(TOUCH_MODE_TOUCHPAD);
}

int chdir(const char* path) {
    return 0;
}

char* vita_prepend_path(const char* path) {
    char* new_path = malloc(2 * FILE_NAME_MAX * sizeof(char));
    snprintf(new_path, 2 * FILE_NAME_MAX * sizeof(char), "%s%s", VITA_PATH_PREFIX, path);

    logs::info("vita_prepend_path: %s", new_path);

    return new_path;
}
