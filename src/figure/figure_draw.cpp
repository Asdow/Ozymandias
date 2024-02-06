#include "core/string.h"

#include "dev/debug.h"
#include "graphics/view/lookup.h"
#include "config/config.h"
#include "figuretype/editor.h"
#include "formation.h"
#include "graphics/graphics.h"
#include "graphics/text.h"
#include "image.h"
#include "game/game.h"

static const vec2i crowd_offsets[] = {
    {0, 0}, {3, 0}, {3, 3}, {-3, 6}, {-3, -3},
    {0, -6}, {6, 0}, {0, 6}, {-6, 0}, {3, -6},
    {-3, 6}, {6, 3}, {-6, -3}, {6, -3}, {-6, 3},
    {3, 6}, {-3, -6}, {0,-10}, {10,0}, {0, 10}, {-10, 0}
};
constexpr int crowd_offsets_size = (int)std::size(crowd_offsets);

void figure::draw_fort_standard(vec2i pixel, int highlight, vec2i* coord_out) {
    painter ctx = game.painter();
    if (!formation_get(formation_id)->in_distant_battle) {
        // base
        ImageDraw::img_generic(ctx, sprite_image_id, pixel.x, pixel.y);
        // flag
        int flag_height = image_get(cart_image_id)->height;
        ImageDraw::img_generic(ctx, cart_image_id, pixel.x, pixel.y - flag_height);
        // top icon
        int icon_image_id = image_id_from_group(GROUP_FIGURE_FORT_STANDARD_ICONS) + formation_get(formation_id)->legion_id;
        ImageDraw::img_generic(ctx, icon_image_id, pixel.x, pixel.y - image_get(icon_image_id)->height - flag_height);
    }
}

void figure::draw_map_flag(vec2i pixel, int highlight, vec2i* coord_out) {
    painter ctx = game.painter();
    // base
    ImageDraw::img_generic(ctx, sprite_image_id, pixel.x, pixel.y);
    // flag
    ImageDraw::img_generic(ctx, cart_image_id, pixel.x, pixel.y - image_get(cart_image_id)->height);
    // flag number
    int number = 0;
    int id = resource_id;
    if (id >= MAP_FLAG_INVASION_MIN && id < MAP_FLAG_INVASION_MAX) {
        number = id - MAP_FLAG_INVASION_MIN + 1;
    } else if (id >= MAP_FLAG_FISHING_MIN && id < MAP_FLAG_FISHING_MAX) {
        number = id - MAP_FLAG_FISHING_MIN + 1;
    } else if (id >= MAP_FLAG_HERD_MIN && id < MAP_FLAG_HERD_MAX) {
        number = id - MAP_FLAG_HERD_MIN + 1;
    }

    if (number > 0) {
        text_draw_number_colored(number, '@', " ", pixel.x + 6, pixel.y + 7, FONT_SMALL_PLAIN, COLOR_WHITE);
    }
}

vec2i figure::adjust_pixel_offset(const vec2i &pixel) {
    // determining x/y offset on tile
    vec2i offset(0, 0);
    if (use_cross_country) {
        auto cc_offets = tile_pixel_coords();
        offset = cc_offets;
        offset.y -= missile_damage;
    } else {
        int dir = figure_image_normalize_direction(direction);
        int adjusted_progress = progress_on_tile;
        if (progress_on_tile >= 8) {
            adjusted_progress -= 15;
        }

        switch (dir) {
        case DIR_0_TOP_RIGHT:
            offset.x += 2 * adjusted_progress;
            offset.y -= adjusted_progress;
            break;
        case DIR_1_RIGHT:
            offset.x += 4 * adjusted_progress;
            offset.y = 0;
            break;
        case DIR_2_BOTTOM_RIGHT:
            offset.x += 2 * adjusted_progress;
            offset.y += adjusted_progress;
            break;
        case DIR_3_BOTTOM:
            offset.x = 0;
            offset.y += 2 * adjusted_progress;
            break;
        case DIR_4_BOTTOM_LEFT:
            offset.x -= 2 * adjusted_progress;
            offset.y += adjusted_progress;
            break;
        case DIR_5_LEFT:
            offset.x -= 4 * adjusted_progress;
            offset.y = 0;
            break;
        case DIR_6_TOP_LEFT:
            offset.x -= 2 * adjusted_progress;
            offset.y -= adjusted_progress;
            break;
        case DIR_7_TOP:
            offset.x = 0;
            offset.y -= 2 * adjusted_progress;
            break;
        }
        offset.y -= current_height;
    }

    if (config_get(CONFIG_GP_CH_CITIZEN_ROAD_OFFSET) && id && type != FIGURE_BALLISTA) {
        // an attempt to not let people walk through each other
        offset += crowd_offsets[id % crowd_offsets_size];
    }

    return {pixel.x + offset.x + 29, pixel.y + offset.y + 15 + 8};
}

void figure::draw_figure_main(painter &ctx, vec2i pixel, int highlight, vec2i* coord_out) {
    int x_correction = 0;
    int y_correction = 3;

    switch (type) {
    case FIGURE_IMMIGRANT: // todo: mmmmmmaybe I was using the wrong draw function all along. look into this
        y_correction = 10;
        break;
        //        case FIGURE_TRADE_CARAVAN:
        //        case FIGURE_TRADE_CARAVAN_DONKEY:
        //            _x = 20;
        //            _y = -4;
        //            break;
    }

    const image_t* img = is_enemy_image ? image_get_enemy(sprite_image_id) : image_get(sprite_image_id);
    ImageDraw::img_sprite(ctx, sprite_image_id, pixel.x + x_correction, pixel.y + y_correction, COLOR_MASK_NONE);
}
void figure::draw_figure_cart(painter &ctx, vec2i pixel, int highlight, vec2i* coord_out) {
    const image_t* img = image_get(cart_image_id);
    ImageDraw::img_sprite(ctx, cart_image_id, pixel.x + cart_offset.x, pixel.y + cart_offset.y - 7);
}
void figure::draw_figure_with_cart(painter &ctx, vec2i pixel, int highlight, vec2i* coord_out) {
    draw_figure_cart(ctx, pixel, highlight, coord_out);
    draw_figure_main(ctx, pixel, highlight, coord_out);
    return; // pharaoh doesn't draw carts on top - to rework maybe later..?

    if (cart_offset.y >= 0) {
        draw_figure_main(ctx, pixel, highlight, coord_out);
        draw_figure_cart(ctx, pixel, highlight, coord_out);
    } else {
        draw_figure_cart(ctx, pixel, highlight, coord_out);
        draw_figure_main(ctx, pixel, highlight, coord_out);
    }
}

void figure::city_draw_figure(painter &ctx, int highlight, vec2i* coord_out) {
    // This is to update the sprite's direction when rotating the city view.
    // Unfortunately, because the only thing we have at the time of file loading is
    // the raw sprite image id, it doesn't work if we haven't performed at least a
    // single frame of figure action after loading a file (i.e. if paused instantly)
    figure_image_update(true);

    if (coord_out != nullptr) {
        highlight = 0;
        *coord_out = cached_pos;
    }

    if (cart_image_id) {
        switch (type) {
        case FIGURE_CART_PUSHER:
        case FIGURE_STORAGE_YARD_DELIVERCART:
            //            case FIGURE_LION_TAMER:
        case FIGURE_DOCKER:
        case FIGURE_NATIVE_TRADER:
            //            case FIGURE_IMMIGRANT:
            //            case FIGURE_EMIGRANT:
            draw_figure_with_cart(ctx, cached_pos, highlight, coord_out);
            break;
            //            case FIGURE_HIPPODROME_HORSES:
            //                hippodrome_horse_adjust(&x, &y, wait_ticks_missile);
            //                draw_figure_with_cart(pixel, highlight, coord_out);
            //                break;
        case FIGURE_STANDARD_BEARER:
            draw_fort_standard(cached_pos, highlight, coord_out);
            break;
        case FIGURE_MAP_FLAG:
            draw_map_flag(cached_pos, highlight, coord_out);
            break;
        default:
            draw_figure_main(ctx, cached_pos, highlight, coord_out);
            break;
        }
    } else {
        draw_figure_main(ctx, cached_pos, highlight, coord_out);
        if (!is_enemy_image && highlight)
            ImageDraw::img_sprite(ctx, sprite_image_id, cached_pos.x, cached_pos.y, COLOR_MASK_LEGION_HIGHLIGHT);
    }

    is_drawn = true;
}