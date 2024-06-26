#include "app.h"

#include "game/settings.h"
#include "platform/screen.h"
#include "core/log.h"
#include <SDL.h>

void app_window_resize(const vec2i& wsize) {
    static int s_width;
    static int s_height;

    s_width = wsize.x;
    s_height = wsize.y;
    SDL_Event event;
    event.user.type = SDL_USEREVENT;
    event.user.code = USER_EVENT_RESIZE;
    event.user.data1 = &s_width;
    event.user.data2 = &s_height;
    SDL_PushEvent(&event);
}

void app_fullscreen(bool fullscreen) {
    app_post_event(fullscreen ? USER_EVENT_FULLSCREEN : USER_EVENT_WINDOWED);
    if (!fullscreen) {
        app_window_resize({1200, 800});
    }
}

void app_post_event(int code) {
    SDL_Event event;
    event.user.type = SDL_USEREVENT;
    event.user.code = code;
    SDL_PushEvent(&event);
}

void app_request_exit() {
    app_post_event(USER_EVENT_QUIT);
}

void app_terminate(const char* message) noexcept {
    logs::critical("%s", message);
    platform_screen_show_error_message_box("CRASHED", message);

    std::terminate();
}
