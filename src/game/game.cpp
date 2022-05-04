#include <scenario/events.h>
#include <window/city.h>
#include "game.h"

#include "building/construction/build_planner.h"
#include "building/model.h"
#include "core/config.h"
#include "core/hotkey_config.h"
#include "core/image.h"
#include "core/lang.h"
#include "core/locale.h"
#include "core/log.h"
#include "core/mods.h"
#include "core/random.h"
#include "core/time.h"
#include "editor/editor.h"
#include "figure/type.h"
#include "game/animation.h"
#include "game/io/boilerplate.h"
#include "game/file_editor.h"
#include "game/settings.h"
#include "game/state.h"
#include "game/tick.h"
#include "graphics/font.h"
#include "graphics/video.h"
#include "graphics/window.h"
#include "input/cursor.h"
#include "input/scroll.h"
#include "scenario/property.h"
#include "scenario/scenario.h"
#include "sound/city.h"
#include "sound/system.h"
#include "translation/translation.h"
#include "window/editor/map.h"
#include "window/logo.h"
#include "window/main_menu.h"
#include "mission.h"

static const time_millis MILLIS_PER_TICK_PER_SPEED[] = {
        0, 20, 35, 55, 80, 110, 160, 240, 350, 500, 700
};

static time_millis last_update;

static void errlog(const char *msg) {
    log_error(msg, 0, 0);
}
static int is_unpatched(void) {
    const uint8_t *delete_game = lang_get_string(1, 6);
    const uint8_t *option_menu = lang_get_string(2, 0);
    const uint8_t *difficulty_option = lang_get_string(2, 6);
    const uint8_t *help_menu = lang_get_string(3, 0);
    // Without patch, the difficulty option string does not exist and
    // getting it "falls through" to the next text group, or, for some
    // languages (pt_BR): delete game falls through to option menu
    return difficulty_option == help_menu || delete_game == option_menu;
}
static encoding_type update_encoding(void) {
    int language = locale_determine_language();
    encoding_type encoding = encoding_determine(language);
    log_info("Detected encoding:", 0, encoding);
    font_set_encoding(encoding);
    translation_load(language);
    return encoding;
}
static bool reload_language(int is_editor, int reload_images) {
    if (!lang_load(is_editor)) {
        if (is_editor)
            errlog("'c3_map.eng' or 'c3_map_mm.eng' files not found or too large.");
        else
            errlog("'c3.eng' or 'c3_mm.eng' files not found or too large.");
        return false;
    }
    encoding_type encoding = update_encoding();

    if (!image_set_font_pak(encoding)) {
        errlog("unable to load font graphics");
        return false;
    }
    if (!image_load_main_paks(CLIMATE_CENTRAL, is_editor, reload_images)) {
        errlog("unable to load main graphics");
        return false;
    }

    return true;
}

static int get_elapsed_ticks(void) {
    if (game_state_is_paused() || !city_has_loaded)
        return 0;

    int game_speed_index = 0;
    int ticks_per_frame = 1;
    switch (window_get_id()) {
        default:
            return 0;
        case WINDOW_CITY:
        case WINDOW_CITY_MILITARY:
        case WINDOW_SLIDING_SIDEBAR:
        case WINDOW_OVERLAY_MENU:
        case WINDOW_BUILD_MENU:
            game_speed_index = (100 - setting_game_speed()) / 10;
            if (game_speed_index >= 10)
                return 0;
            else if (game_speed_index < 0) {
                ticks_per_frame = setting_game_speed() / 100;
                game_speed_index = 0;
            }
            break;
        case WINDOW_EDITOR_MAP:
            game_speed_index = 3; // 70%, nice speed for flag animations
            break;
    }
    if (Planner.in_progress)
        return 0;

    if (scroll_in_progress() && !scroll_is_smooth())
        return 0;

    time_millis now = time_get_millis();
    time_millis diff = now - last_update;
    if (diff < MILLIS_PER_TICK_PER_SPEED[game_speed_index] + 2)
        return 0;

    last_update = now;
    return ticks_per_frame;
}

bool game_pre_init(void) {
    if (!lang_load(0))
        return false;
    update_encoding();
    settings_load(); // c3.inf
    config_load(); // augustus.ini
    hotkey_config_load();
    scenario_settings_init();
    random_init();

    game_state_unpause();
    return true;
}
bool game_init(void) {
    image_data_init();
    if (!image_load_main_paks(CLIMATE_CENTRAL, 0, 0)) {
        errlog("unable to load main graphics");
        return false;
    }
    int missing_fonts = 0;
    if (!image_set_font_pak(encoding_get())) {
        errlog("unable to load font graphics");
        if (encoding_get() == ENCODING_KOREAN)
            missing_fonts = 1;
        else
            return false;
    }

    if (!model_load()) {
        errlog("unable to load model data");
        return false;
    }

    if (GAME_ENV == ENGINE_ENV_PHARAOH) {
        if (!eventmsg_load()) {
            errlog("unable to load eventmsg.txt");
            return false;
        }
        if (!eventmsg_auto_phrases_load()) {
            errlog("unable to load event auto reason phrases");
            return false;
        }
        if (!game_load_campaign_file()) {
            errlog("unable to load campaign data");
            return false;
        }
    }

//    mods_init();
    sound_system_init();
    game_state_init();
    window_logo_show(missing_fonts ? MESSAGE_MISSING_FONTS : (is_unpatched() ? MESSAGE_MISSING_PATCH : MESSAGE_NONE));

    return true;
}
bool game_init_editor(void) {
    if (!reload_language(1, 0))
        return false;

    game_file_editor_clear_data();
    game_file_editor_create_scenario(2);

    if (city_view_is_sidebar_collapsed())
        city_view_toggle_sidebar();

    editor_set_active(1);
    window_editor_map_show();
    return true;
}
void game_exit_editor(void) {
    if (!reload_language(0, 0))
        return;
    editor_set_active(0);
    window_main_menu_show(1);
}
int game_reload_language(void) {
    return reload_language(0, 1);
}
void game_run(void) {
    game_animation_update();
    int num_ticks = get_elapsed_ticks();
    for (int i = 0; i < num_ticks; i++) {
        game_tick_run();

        if (window_is_invalid())
            break;
    }
    if (window_is(WINDOW_CITY))
        anti_scum_random_uint16();
}
void game_draw(void) {
    window_draw(0);
    sound_city_play();
}
void game_exit(void) {
    video_shutdown();
    settings_save();
    config_save();
    sound_system_shutdown();
}
