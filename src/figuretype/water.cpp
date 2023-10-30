#include "figuretype/water.h"

#include "building/building.h"
#include "building/model.h"
#include "city/gods.h"
#include "city/message.h"
#include "core/calc.h"
#include "core/random.h"
#include "figure/image.h"
#include "figure/movement.h"
#include "figure/route.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "grid/figure.h"
#include "grid/grid.h"
#include "grid/water.h"
#include "scenario/map.h"

static const int FLOTSAM_RESOURCE_IDS[] = {3, 1, 3, 2, 1, 3, 2, 3, 2, 1, 3, 3, 2, 3, 3, 3, 1, 2, 0, 1};
static const int FLOTSAM_WAIT_TICKS[]  = {10, 50, 100, 130, 200, 250, 400, 430, 500, 600, 70, 750, 820, 830, 900, 980, 1010, 1030, 1200, 1300};

static const int FLOTSAM_TYPE_0[] = {0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0};
static const int FLOTSAM_TYPE_12[] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 3, 2, 1, 0, 0, 1, 1, 2, 2, 1, 1, 0, 0, 0};
static const int FLOTSAM_TYPE_3[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

void figure_create_flotsam(void) {
    if (!scenario_map_has_river_entry() || !scenario_map_has_river_exit() || !scenario_map_has_flotsam())
        return;
    for (int i = 1; i < MAX_FIGURES[GAME_ENV]; i++) {
        figure* f = figure_get(i);
        if (f->state && f->type == FIGURE_FLOTSAM)
            f->poof();
    }

    map_point river_entry = scenario_map_river_entry();
    for (int i = 0; i < 20; i++) {
        figure* f = figure_create(FIGURE_FLOTSAM, river_entry, DIR_0_TOP_RIGHT);
        f->action_state = FIGURE_ACTION_128_FLOTSAM_CREATED;
        f->set_resource((e_resource)FLOTSAM_RESOURCE_IDS[i]);
        f->wait_ticks = FLOTSAM_WAIT_TICKS[i];
    }
}
void figure_sink_all_ships(void) {
    for (int i = 1; i < MAX_FIGURES[GAME_ENV]; i++) {
        figure* f = figure_get(i);
        if (f->state != FIGURE_STATE_ALIVE)
            continue;

        if (f->type == FIGURE_TRADE_SHIP)
            f->destination()->data.dock.trade_ship_id = 0;
        else if (f->type == FIGURE_FISHING_BOAT)
            f->home()->data.industry.fishing_boat_id = 0;
        else {
            continue;
        }
        f->set_home(0);
        f->type = FIGURE_SHIPWRECK;
        f->wait_ticks = 0;
    }
}

void figure::flotsam_action() {
    allow_move_type = EMOVE_FLOTSAM;
    if (!scenario_map_has_river_exit())
        return;
    //    is_ghost = false;
    cart_image_id = 0;
    terrain_usage = TERRAIN_USAGE_ANY;
    switch (action_state) {
    case FIGURE_ACTION_128_FLOTSAM_CREATED:
        //            is_ghost = true;
        wait_ticks--;
        if (wait_ticks <= 0) {
            action_state = FIGURE_ACTION_129_FLOTSAM_FLOATING;
            wait_ticks = 0;
            if (!resource_id && city_god_osiris_create_shipwreck_flotsam())
                min_max_seen = 1;

            destination_tile = scenario_map_river_exit();
            //                map_point river_exit = scenario_map_river_exit();
            //                destination_tile.x() = river_exit.x();
            //                destination_tile.y() = river_exit.y();
        }
        break;
    case FIGURE_ACTION_129_FLOTSAM_FLOATING:
        if (flotsam_visible)
            flotsam_visible = 0;
        else {
            flotsam_visible = 1;
            wait_ticks++;
            move_ticks(1);
            //                is_ghost = false;
            height_adjusted_ticks = 0;
            if (direction == DIR_FIGURE_NONE || direction == DIR_FIGURE_REROUTE
                || direction == DIR_FIGURE_CAN_NOT_REACH) {
                action_state = FIGURE_ACTION_130_FLOTSAM_OFF_MAP;
            }
        }
        break;
    case FIGURE_ACTION_130_FLOTSAM_OFF_MAP:
        //            is_ghost = true;
        min_max_seen = 0;
        action_state = FIGURE_ACTION_128_FLOTSAM_CREATED;
        if (wait_ticks >= 400)
            wait_ticks = random_byte() & 7;
        else if (wait_ticks >= 200)
            wait_ticks = 50 + (random_byte() & 0xf);
        else if (wait_ticks >= 100)
            wait_ticks = 100 + (random_byte() & 0x1f);
        else if (wait_ticks >= 50)
            wait_ticks = 200 + (random_byte() & 0x3f);
        else {
            wait_ticks = 300 + random_byte();
        }
        map_figure_remove();
        tile = scenario_map_river_entry();
        //            map_point river_entry = scenario_map_river_entry();
        //            tile.x() = river_entry.x();
        //            tile.y() = river_entry.y();
        //            tile.grid_offset() = MAP_OFFSET(tile.x(), tile.y());
        cc_coords.x = 15 * tile.x();
        cc_coords.y = 15 * tile.y();
        break;
    }
    if (resource_id == 0) {
        //        figure_image_increase_offset(12);
        if (min_max_seen)
            sprite_image_id = image_id_from_group(GROUP_FIGURE_FLOTSAM_SHEEP) + FLOTSAM_TYPE_0[anim_frame];
        else {
            sprite_image_id = image_id_from_group(GROUP_FIGURE_FLOTSAM_0) + FLOTSAM_TYPE_0[anim_frame];
        }
    } else if (resource_id == 1) {
        //        figure_image_increase_offset(24);
        sprite_image_id = image_id_from_group(GROUP_FIGURE_FLOTSAM_1) + FLOTSAM_TYPE_12[anim_frame];
    } else if (resource_id == 2) {
        //        figure_image_increase_offset(24);
        sprite_image_id = image_id_from_group(GROUP_FIGURE_FLOTSAM_2) + FLOTSAM_TYPE_12[anim_frame];
    } else if (resource_id == 3) {
        //        figure_image_increase_offset(24);
        if (FLOTSAM_TYPE_3[anim_frame] == -1)
            sprite_image_id = 0;
        else {
            sprite_image_id = image_id_from_group(GROUP_FIGURE_FLOTSAM_3) + FLOTSAM_TYPE_3[anim_frame];
        }
    }
}

void figure::shipwreck_action() {
    //    is_ghost = false;
    height_adjusted_ticks = 0;
    allow_move_type = EMOVE_BOAT;
    //    figure_image_increase_offset(128);
    if (wait_ticks < 1000) {
        map_figure_remove();
        map_point shipwreck_tile;
        if (map_water_find_shipwreck_tile(this, &shipwreck_tile)) {
            tile = shipwreck_tile;
            //            tile.x() = tile.x();
            //            tile.y() = tile.y();
            //            tile.grid_offset() = MAP_OFFSET(tile.x(), tile.y());
            cc_coords.x = 15 * tile.x() + 7;
            cc_coords.y = 15 * tile.y() + 7;
        }
        map_figure_add();
        wait_ticks = 1000;
    }
    wait_ticks++;
    if (wait_ticks > 2000)
        poof();

    sprite_image_id = image_id_from_group(GROUP_FIGURE_SHIPWRECK) + anim_frame / 16;
}


