#include "city_overlay_fire.h"

#include "building/model.h"
#include "game/state.h"
#include "grid/building.h"
#include "grid/property.h"
#include "figure/figure.h"

city_overlay_fire g_city_overlay_fire;

city_overlay* city_overlay_for_fire() {
    return &g_city_overlay_fire;
}

city_overlay_fire::city_overlay_fire() {
    type = OVERLAY_FIRE;
    column_type = COLUMN_TYPE_RISK;
}

bool city_overlay_fire::show_figure(const figure *f) const {
    return f->type == FIGURE_FIREMAN;
}

void city_overlay_fire::draw_custom_top(vec2i pixel, tile2i point, painter &ctx) const {
    if (!map_property_is_draw_tile(point)) {
        return;
    }

    if (map_building_at(point)) {
        city_overlay_fire::draw_building_top(pixel, point, ctx);
    }
}

int city_overlay_fire::get_column_height(const building *b) const {
    auto model = model_get_building(b->type);

    if (b->prev_part_building_id || !model->fire_risk)
        return NO_COLUMN;

    if (b->type == BUILDING_HOUSE_VACANT_LOT || b->type == BUILDING_GARDENS
        || b->type == BUILDING_BANDSTAND || b->type == BUILDING_BOOTH) {
        return NO_COLUMN;
    }

    return b->fire_risk / 100;
}

int city_overlay_fire::get_tooltip_for_building(tooltip_context *c, const building *b) const {
    if (b->fire_risk <= 0)
        return 46;
    else if (b->fire_risk <= 200)
        return 47;
    else if (b->fire_risk <= 400)
        return 48;
    else if (b->fire_risk <= 600)
        return 49;
    else if (b->fire_risk <= 800)
        return 50;
    else
        return 51;
}

bool city_overlay_fire::show_building(const building *b) const {
    return b->type == BUILDING_FIREHOUSE || b->type == BUILDING_BURNING_RUIN || b->type == BUILDING_FESTIVAL_SQUARE;
}
