#include "building_plaza.h"

#include "building/building.h"
#include "city/object_info.h"
#include "city/buildings.h"
#include "core/calc.h"
#include "game/resource.h"
#include "graphics/elements/panel.h"
#include "graphics/elements/lang_text.h"
#include "graphics/boilerplate.h"
#include "graphics/text.h"
#include "io/gamefiles/lang.h"
#include "config/config.h"
#include "window/building/common.h"
#include "window/building/figures.h"
#include "sound/sound_building.h"

void building_shipyard_draw_info(object_info &c) {
    c.help_id = 82;
    window_building_play_sound(&c, snd::get_building_info_sound("shipyard"));
    outer_panel_draw(c.x_offset, c.y_offset, c.width_blocks, c.height_blocks);
    lang_text_draw_centered(100, 0, c.x_offset, c.y_offset + 10, 16 * c.width_blocks, FONT_LARGE_BLACK_ON_LIGHT);

    building* b = building_get(c.building_id);

    if (!c.has_road_access) {
        window_building_draw_description(c, 69, 25);
    } else {
        int pct_done = calc_percentage(b->data.industry.progress, 160);
        int width = lang_text_draw(100, 2, c.x_offset + 32, c.y_offset + 56, FONT_NORMAL_BLACK_ON_LIGHT);
        width += text_draw_percentage(pct_done, c.x_offset + 32 + width, c.y_offset + 56, FONT_NORMAL_BLACK_ON_LIGHT);
        lang_text_draw(100, 3, c.x_offset + 32 + width, c.y_offset + 56, FONT_NORMAL_BLACK_ON_LIGHT);

        if (city_buildings_shipyard_boats_requested()) {
            lang_text_draw_multiline(100, 5, c.x_offset + 32, c.y_offset + 80, 16 * (c.width_blocks - 6), FONT_NORMAL_BLACK_ON_LIGHT);
        } else {
            lang_text_draw_multiline(100, 4, c.x_offset + 32, c.y_offset + 80, 16 * (c.width_blocks - 6), FONT_NORMAL_BLACK_ON_LIGHT);
        }
    }

    inner_panel_draw(c.x_offset + 16, c.y_offset + 136, c.width_blocks - 2, 4);
    window_building_draw_employment(&c, 142);
}