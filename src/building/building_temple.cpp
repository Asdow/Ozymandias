#include "building_plaza.h"

#include "building/building.h"
#include "city/object_info.h"
#include "game/resource.h"
#include "graphics/elements/panel.h"
#include "graphics/elements/lang_text.h"
#include "graphics/view/view.h"
#include "graphics/boilerplate.h"
#include "io/gamefiles/lang.h"
#include "config/config.h"
#include "window/building/common.h"
#include "window/building/figures.h"
#include "sound/sound_building.h"
#include "game/game.h"

static void building_temple_draw_temple(object_info& c, const char* type, int group_id, int image_offset) {
    c.help_id = 67;
    window_building_play_sound(&c, snd::get_building_info_sound(type));
    outer_panel_draw(c.offset, c.width_blocks, c.height_blocks);
    lang_text_draw_centered(group_id, 0, c.offset.x, c.offset.y + 12, 16 * c.width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    inner_panel_draw(c.offset.x + 16, c.offset.y + 56, c.width_blocks - 2, 4);
    window_building_draw_employment(&c, 62);
    painter ctx = game.painter();
    if (c.has_road_access) {
        ImageDraw::img_generic(ctx, image_offset + image_id_from_group(GROUP_PANEL_WINDOWS), c.offset.x + 190, c.offset.y + 16 * c.height_blocks - 118);
    } else {
        window_building_draw_description_at(c, 16 * c.height_blocks - 128, 69, 25);
    }
}

void building_temple_osiris_draw_info(object_info& c) {
    building_temple_draw_temple(c, "temple_osiris", 92, 21);
}
void building_temple_ra_draw_info(object_info& c) {
    building_temple_draw_temple(c, "temple_ra", 93, 22);
}
void building_temple_ptah_draw_info(object_info& c) {
    building_temple_draw_temple(c, "temple_ptah", 94, 23);
}
void building_temple_seth_draw_info(object_info& c) {
    building_temple_draw_temple(c, "temple_seth", 95, 24);
}
void building_temple_bast_draw_info(object_info& c) {
    building_temple_draw_temple(c, "temple_bast", 96, 25);
}
