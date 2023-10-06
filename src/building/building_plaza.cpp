#include "building_plaza.h"

#include "building/building.h"
#include "city/object_info.h"
#include "game/resource.h"
#include "graphics/elements/panel.h"
#include "graphics/elements/lang_text.h"
#include "graphics/boilerplate.h"
#include "io/gamefiles/lang.h"
#include "config/config.h"
#include "window/building/common.h"
#include "window/building/figures.h"
#include "sound/sound_building.h"

void building_plaza_draw_info(object_info &c) {
    c.help_id = 80;
    window_building_play_sound(&c, snd::get_building_info_sound("plaza"));
    window_building_prepare_figure_list(&c);
    outer_panel_draw(c.x_offset, c.y_offset, c.width_blocks, c.height_blocks);
    lang_text_draw_centered(137, 0, c.x_offset, c.y_offset + 10, 16 * c.width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    window_building_draw_figure_list(&c);
    window_building_draw_description_at(c, 16 * c.height_blocks - 113, 137, 1);
}