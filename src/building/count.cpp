#include "count.h"

#include "building/building.h"
#include "city/buildings.h"
#include "city/health.h"
#include "figure/figure.h"
#include "core/game_environment.h"

#include <string.h>

struct record {
    int active;
    int total;
};

static struct {
    struct record buildings[int_MAX];
    struct record industry[36];
} data;

static void clear_counters(void) {
    memset(&data, 0, sizeof(data));
}
static void increase_count(int type, int active) {
    ++data.buildings[type].total;
    if (active)
        ++data.buildings[type].active;

}
static void increase_industry_count(int resource, int active) {
    ++data.industry[resource].total;
    if (active)
        ++data.industry[resource].active;

}

static void limit_hippodrome(void) {
    if (data.buildings[BUILDING_HIPPODROME].total > 1)
        data.buildings[BUILDING_HIPPODROME].total = 1;
    if (data.buildings[BUILDING_HIPPODROME].active > 1)
        data.buildings[BUILDING_HIPPODROME].active = 1;
}

void building_entertainment_update() {
    for (int i = 1; i < MAX_BUILDINGS[GAME_ENV]; i++) {
        building *b = building_get(i);
        if (b->state != BUILDING_STATE_VALID || b->house_size)
            continue;

        int is_entertainment_venue = 0;
        int type = b->type;
        switch (type) {
            // SPECIAL TREATMENT
            // entertainment venues
            case BUILDING_THEATER:
            case BUILDING_AMPHITHEATER:
            case BUILDING_COLOSSEUM:
            case BUILDING_HIPPODROME:
                is_entertainment_venue = 1;
                break;
        }
        if (is_entertainment_venue) {
            // update number of shows
            int shows = 0;
            if (b->data.entertainment.days1 > 0) {
                --b->data.entertainment.days1;
                ++shows;
            }
            if (b->data.entertainment.days2 > 0) {
                --b->data.entertainment.days2;
                ++shows;
            }
            if (type != BUILDING_THEATER && b->data.entertainment.days3_or_play > 0) {
                --b->data.entertainment.days3_or_play;
                ++shows;
            }
            b->data.entertainment.num_shows = shows;
        }
    }
}
void building_count_update(void) {
    clear_counters();
    city_buildings_reset_dock_wharf_counters();
    city_health_reset_hospital_workers();

    for (int i = 1; i < MAX_BUILDINGS[GAME_ENV]; i++) {
        building *b = building_get(i);
        if (b->state != BUILDING_STATE_VALID || b->house_size)
            continue;

        int type = b->type;
        switch (type) {
            // SPECIAL TREATMENT
            // entertainment venues
            case BUILDING_THEATER:
                increase_count(type, b->num_workers > 0);
                break;
            case BUILDING_AMPHITHEATER:
                increase_count(type, b->num_workers > 0);
                increase_count(BUILDING_THEATER, b->num_workers > 0);
                break;
            case BUILDING_COLOSSEUM:
                increase_count(type, b->num_workers > 0);
                increase_count(BUILDING_AMPHITHEATER, b->num_workers > 0);
                increase_count(BUILDING_THEATER, b->num_workers > 0);
                break;
            case BUILDING_HIPPODROME:
                increase_count(type, b->num_workers > 0);
                break;

            case BUILDING_BARRACKS:
                city_buildings_set_barracks(i);
                increase_count(type, b->num_workers > 0);
                break;

            case BUILDING_HOSPITAL:
                increase_count(type, b->num_workers > 0);
                city_health_add_hospital_workers(b->num_workers);
                break;

                // water
            case BUILDING_RESERVOIR:
            case BUILDING_FOUNTAIN:
                increase_count(type, b->has_water_access);
                break;

                // DEFAULT TREATMENT
                // education
            case BUILDING_SCHOOL:
            case BUILDING_LIBRARY:
            case BUILDING_ACADEMY:
                // health
            case BUILDING_BARBER:
            case BUILDING_BATHHOUSE:
            case BUILDING_DOCTOR:
                // government
            case BUILDING_FORUM:
            case BUILDING_FORUM_UPGRADED:
            case BUILDING_SENATE:
            case BUILDING_SENATE_UPGRADED:
                // entertainment schools
            case BUILDING_ACTOR_COLONY:
            case BUILDING_GLADIATOR_SCHOOL:
            case BUILDING_LION_HOUSE:
            case BUILDING_CHARIOT_MAKER:
                // distribution
            case BUILDING_MARKET:
                // military
            case BUILDING_MILITARY_ACADEMY:
                // religion
            case BUILDING_SMALL_TEMPLE_CERES:
            case BUILDING_SMALL_TEMPLE_NEPTUNE:
            case BUILDING_SMALL_TEMPLE_MERCURY:
            case BUILDING_SMALL_TEMPLE_MARS:
            case BUILDING_SMALL_TEMPLE_VENUS:
            case BUILDING_LARGE_TEMPLE_CERES:
            case BUILDING_LARGE_TEMPLE_NEPTUNE:
            case BUILDING_LARGE_TEMPLE_MERCURY:
            case BUILDING_LARGE_TEMPLE_MARS:
            case BUILDING_LARGE_TEMPLE_VENUS:
            case BUILDING_ORACLE:
                increase_count(type, b->num_workers > 0);
                break;

            case BUILDING_SHRINE_OSIRIS:
            case BUILDING_SHRINE_RA:
            case BUILDING_SHRINE_PTAH:
            case BUILDING_SHRINE_SETH:
            case BUILDING_SHRINE_BAST:
                increase_count(type, b->has_road_access > 0);
                break;

                // water-side
            case BUILDING_WHARF:
                if (b->num_workers > 0)
                    city_buildings_add_working_wharf(!b->data.industry.fishing_boat_id);
                break;

            case BUILDING_DOCK:
                if (b->num_workers > 0 && b->has_water_access)
                    city_buildings_add_working_dock(i);
                break;
        }
        // industry
        if (GAME_ENV == ENGINE_ENV_C3) {
            switch (b->type) {
                case BUILDING_WHEAT_FARM:
                    increase_industry_count(RESOURCE_WHEAT, b->num_workers > 0);
                    break;
                case BUILDING_VEGETABLE_FARM:
                    increase_industry_count(RESOURCE_VEGETABLES, b->num_workers > 0);
                    break;
                case BUILDING_FRUIT_FARM:
                    increase_industry_count(RESOURCE_FRUIT, b->num_workers > 0);
                    break;
                case BUILDING_OLIVE_FARM:
                    increase_industry_count(RESOURCE_OLIVES, b->num_workers > 0);
                    break;
                case BUILDING_VINES_FARM:
                    increase_industry_count(RESOURCE_VINES, b->num_workers > 0);
                    break;
                case BUILDING_PIG_FARM:
                    increase_industry_count(RESOURCE_MEAT_C3, b->num_workers > 0);
                    break;
                case BUILDING_MARBLE_QUARRY:
                    increase_industry_count(RESOURCE_MARBLE_C3, b->num_workers > 0);
                    break;
                case BUILDING_IRON_MINE:
                    increase_industry_count(RESOURCE_IRON, b->num_workers > 0);
                    break;
                case BUILDING_TIMBER_YARD:
                    increase_industry_count(RESOURCE_TIMBER_C3, b->num_workers > 0);
                    break;
                case BUILDING_CLAY_PIT:
                    increase_industry_count(RESOURCE_CLAY, b->num_workers > 0);
                    break;
                case BUILDING_WINE_WORKSHOP:
                    increase_industry_count(RESOURCE_WINE, b->num_workers > 0);
                    break;
                case BUILDING_OIL_WORKSHOP:
                    increase_industry_count(RESOURCE_OIL_C3, b->num_workers > 0);
                    break;
                case BUILDING_WEAPONS_WORKSHOP:
                    increase_industry_count(RESOURCE_WEAPONS_C3, b->num_workers > 0);
                    break;
                case BUILDING_FURNITURE_WORKSHOP:
                    increase_industry_count(RESOURCE_FURNITURE, b->num_workers > 0);
                    break;
                case BUILDING_POTTERY_WORKSHOP:
                    increase_industry_count(RESOURCE_POTTERY_C3, b->num_workers > 0);
                    break;
                default:
                    continue;
            }
        } else if (GAME_ENV == ENGINE_ENV_PHARAOH) {
            switch (b->type) {
                case BUILDING_GRAIN_FARM:
                    increase_industry_count(RESOURCE_GRAIN, b->num_workers > 0); break;
//                    increase_industry_count(RESOURCE_STRAW, b->num_workers > 0); break; // TODO: how is this handled in Pharaoh??
                case BUILDING_BARLEY_FARM:
                    increase_industry_count(RESOURCE_BARLEY, b->num_workers > 0); break;
                case BUILDING_FLAX_FARM:
                    increase_industry_count(RESOURCE_FLAX, b->num_workers > 0); break;
                case BUILDING_LETTUCE_FARM:
                    increase_industry_count(RESOURCE_LETTUCE, b->num_workers > 0); break;
                case BUILDING_POMEGRANATES_FARM:
                    increase_industry_count(RESOURCE_POMEGRANATES, b->num_workers > 0); break;
                case BUILDING_CHICKPEAS_FARM:
                    increase_industry_count(RESOURCE_CHICKPEAS, b->num_workers > 0); break;
                case BUILDING_FIGS_FARM:
                    increase_industry_count(RESOURCE_FIGS, b->num_workers > 0); break;
                case BUILDING_HENNA_FARM:
                    increase_industry_count(RESOURCE_HENNA, b->num_workers > 0); break;
                    ////
                case BUILDING_HUNTING_LODGE:
                    increase_industry_count(RESOURCE_GAMEMEAT, b->num_workers > 0); break;
                case BUILDING_FISHING_WHARF:
                    increase_industry_count(RESOURCE_FISH, b->num_workers > 0); break;
                    ////
                case BUILDING_CLAY_PIT:
                    increase_industry_count(RESOURCE_CLAY, b->num_workers > 0); break;
                case BUILDING_REED_GATHERER:
                    increase_industry_count(RESOURCE_REEDS, b->num_workers > 0); break;
                case BUILDING_TIMBER_YARD:
                    increase_industry_count(RESOURCE_TIMBER_PH, b->num_workers > 0); break;
                    ////
                case BUILDING_GEMSTONE_MINE:
                    increase_industry_count(RESOURCE_GEMS, b->num_workers > 0); break;
                case BUILDING_GOLD_MINE:
                    increase_industry_count(RESOURCE_GOLD, b->num_workers > 0); break;
                case BUILDING_COPPER_MINE:
                    increase_industry_count(RESOURCE_COPPER, b->num_workers > 0); break;
                    ////
                case BUILDING_STONE_QUARRY:
                    increase_industry_count(RESOURCE_STONE, b->num_workers > 0); break;
                case BUILDING_LIMESTONE_QUARRY:
                    increase_industry_count(RESOURCE_LIMESTONE, b->num_workers > 0); break;
                case BUILDING_GRANITE_QUARRY:
                    increase_industry_count(RESOURCE_GRANITE, b->num_workers > 0); break;
                case BUILDING_SANDSTONE_QUARRY:
                    increase_industry_count(RESOURCE_SANDSTONE, b->num_workers > 0); break;
                    ////
                case BUILDING_POTTERY_WORKSHOP:
                    increase_industry_count(RESOURCE_POTTERY_PH, b->num_workers > 0); break;
                case BUILDING_BEER_WORKSHOP:
                    increase_industry_count(RESOURCE_BEER, b->num_workers > 0); break;
                case BUILDING_LINEN_WORKSHOP:
                    increase_industry_count(RESOURCE_LINEN, b->num_workers > 0); break;
                case BUILDING_JEWELS_WORKSHOP:
                    increase_industry_count(RESOURCE_LUXURY_GOODS, b->num_workers > 0); break;
                case BUILDING_PAPYRUS_WORKSHOP:
                    increase_industry_count(RESOURCE_PAPYRUS, b->num_workers > 0); break;
                case BUILDING_BRICKS_WORKSHOP:
                    increase_industry_count(RESOURCE_BRICKS, b->num_workers > 0); break;
                case BUILDING_LAMP_WORKSHOP:
                    increase_industry_count(RESOURCE_LAMPS, b->num_workers > 0); break;
                case BUILDING_PAINT_WORKSHOP:
                    increase_industry_count(RESOURCE_PAINT, b->num_workers > 0); break;
//                case BUILDING_OIL_WORKSHOP:
//                    increase_industry_count(RESOURCE_OIL_PH, b->num_workers > 0); break;
                    ////
                case BUILDING_WEAPONS_WORKSHOP:
                    increase_industry_count(RESOURCE_WEAPONS_PH, b->num_workers > 0); break;
                case BUILDING_CHARIOTS_WORKSHOP:
                    increase_industry_count(RESOURCE_CHARIOTS, b->num_workers > 0); break;
                    ////
                default:
                    continue;
            }
        }
        if (b->has_figure(2)) {
            figure *f = b->get_figure(2);
            if (f->state != FIGURE_STATE_ALIVE || f->destination()->id != i)
                b->remove_figure(2);
//                b->immigrant_figure_id = 0;
        }
    }
    if (GAME_ENV == ENGINE_ENV_C3)
        limit_hippodrome();
}
int building_count_active(int type) {
    return data.buildings[type].active;
}
int building_count_total(int type) {
    return data.buildings[type].total;
}
int building_count_industry_active(int resource) {
    return data.industry[resource].active;
}
int building_count_industry_total(int resource) {
    return data.industry[resource].total;
}
void building_count_save_state(buffer *industry, buffer *culture1, buffer *culture2, buffer *culture3, buffer *military, buffer *support) {
    // industry
    for (int i = 0; i < RESOURCE_MAX[GAME_ENV]; i++)
        industry->write_i32(data.industry[i].total);
    for (int i = 0; i < RESOURCE_MAX[GAME_ENV]; i++)
        industry->write_i32(data.industry[i].active);

    // culture 1
    culture1->write_i32(data.buildings[BUILDING_THEATER].total);
    culture1->write_i32(data.buildings[BUILDING_THEATER].active);
    culture1->write_i32(data.buildings[BUILDING_AMPHITHEATER].total);
    culture1->write_i32(data.buildings[BUILDING_AMPHITHEATER].active);
    culture1->write_i32(data.buildings[BUILDING_COLOSSEUM].total);
    culture1->write_i32(data.buildings[BUILDING_COLOSSEUM].active);
    culture1->write_i32(data.buildings[BUILDING_HIPPODROME].total);
    culture1->write_i32(data.buildings[BUILDING_HIPPODROME].active);
    culture1->write_i32(data.buildings[BUILDING_SCHOOL].total);
    culture1->write_i32(data.buildings[BUILDING_SCHOOL].active);
    culture1->write_i32(data.buildings[BUILDING_LIBRARY].total);
    culture1->write_i32(data.buildings[BUILDING_LIBRARY].active);
    culture1->write_i32(data.buildings[BUILDING_ACADEMY].total);
    culture1->write_i32(data.buildings[BUILDING_ACADEMY].active);
    culture1->write_i32(data.buildings[BUILDING_BARBER].total);
    culture1->write_i32(data.buildings[BUILDING_BARBER].active);
    culture1->write_i32(data.buildings[BUILDING_BATHHOUSE].total);
    culture1->write_i32(data.buildings[BUILDING_BATHHOUSE].active);
    culture1->write_i32(data.buildings[BUILDING_DOCTOR].total);
    culture1->write_i32(data.buildings[BUILDING_DOCTOR].active);
    culture1->write_i32(data.buildings[BUILDING_HOSPITAL].total);
    culture1->write_i32(data.buildings[BUILDING_HOSPITAL].active);
    culture1->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_CERES].total);
    culture1->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].total);
    culture1->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].total);
    culture1->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_MARS].total);
    culture1->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_VENUS].total);
    culture1->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_CERES].total);
    culture1->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].total);
    culture1->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].total);
    culture1->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_MARS].total);
    culture1->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_VENUS].total);
    culture1->write_i32(data.buildings[BUILDING_ORACLE].total);

    // culture 2
    culture2->write_i32(data.buildings[BUILDING_ACTOR_COLONY].total);
    culture2->write_i32(data.buildings[BUILDING_ACTOR_COLONY].active);
    culture2->write_i32(data.buildings[BUILDING_GLADIATOR_SCHOOL].total);
    culture2->write_i32(data.buildings[BUILDING_GLADIATOR_SCHOOL].active);
    culture2->write_i32(data.buildings[BUILDING_LION_HOUSE].total);
    culture2->write_i32(data.buildings[BUILDING_LION_HOUSE].active);
    culture2->write_i32(data.buildings[BUILDING_CHARIOT_MAKER].total);
    culture2->write_i32(data.buildings[BUILDING_CHARIOT_MAKER].active);

    // culture 3
    culture3->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_CERES].active);
    culture3->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].active);
    culture3->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].active);
    culture3->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_MARS].active);
    culture3->write_i32(data.buildings[BUILDING_SMALL_TEMPLE_VENUS].active);
    culture3->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_CERES].active);
    culture3->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].active);
    culture3->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].active);
    culture3->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_MARS].active);
    culture3->write_i32(data.buildings[BUILDING_LARGE_TEMPLE_VENUS].active);

    // military
    military->write_i32(data.buildings[BUILDING_MILITARY_ACADEMY].total);
    military->write_i32(data.buildings[BUILDING_MILITARY_ACADEMY].active);
    military->write_i32(data.buildings[BUILDING_BARRACKS].total);
    military->write_i32(data.buildings[BUILDING_BARRACKS].active);

    // support
    support->write_i32(data.buildings[BUILDING_MARKET].total);
    support->write_i32(data.buildings[BUILDING_MARKET].active);
    support->write_i32(data.buildings[BUILDING_RESERVOIR].total);
    support->write_i32(data.buildings[BUILDING_RESERVOIR].active);
    support->write_i32(data.buildings[BUILDING_FOUNTAIN].total);
    support->write_i32(data.buildings[BUILDING_FOUNTAIN].active);
}
void building_count_load_state(buffer *industry, buffer *culture1, buffer *culture2, buffer *culture3, buffer *military, buffer *support) {
    // industry
    for (int i = 0; i < RESOURCE_MAX[GAME_ENV]; i++)
        data.industry[i].total = industry->read_i32();
    for (int i = 0; i < RESOURCE_MAX[GAME_ENV]; i++)
        data.industry[i].active = industry->read_i32();

    // culture 1
    data.buildings[BUILDING_THEATER].total = culture1->read_i32();
    data.buildings[BUILDING_THEATER].active = culture1->read_i32();
    data.buildings[BUILDING_AMPHITHEATER].total = culture1->read_i32();
    data.buildings[BUILDING_AMPHITHEATER].active = culture1->read_i32();
    data.buildings[BUILDING_COLOSSEUM].total = culture1->read_i32();
    data.buildings[BUILDING_COLOSSEUM].active = culture1->read_i32();
    data.buildings[BUILDING_HIPPODROME].total = culture1->read_i32();
    data.buildings[BUILDING_HIPPODROME].active = culture1->read_i32();
    data.buildings[BUILDING_SCHOOL].total = culture1->read_i32();
    data.buildings[BUILDING_SCHOOL].active = culture1->read_i32();
    data.buildings[BUILDING_LIBRARY].total = culture1->read_i32();
    data.buildings[BUILDING_LIBRARY].active = culture1->read_i32();
    data.buildings[BUILDING_ACADEMY].total = culture1->read_i32();
    data.buildings[BUILDING_ACADEMY].active = culture1->read_i32();
    data.buildings[BUILDING_BARBER].total = culture1->read_i32();
    data.buildings[BUILDING_BARBER].active = culture1->read_i32();
    data.buildings[BUILDING_BATHHOUSE].total = culture1->read_i32();
    data.buildings[BUILDING_BATHHOUSE].active = culture1->read_i32();
    data.buildings[BUILDING_DOCTOR].total = culture1->read_i32();
    data.buildings[BUILDING_DOCTOR].active = culture1->read_i32();
    data.buildings[BUILDING_HOSPITAL].total = culture1->read_i32();
    data.buildings[BUILDING_HOSPITAL].active = culture1->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_CERES].total = culture1->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].total = culture1->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].total = culture1->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_MARS].total = culture1->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_VENUS].total = culture1->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_CERES].total = culture1->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].total = culture1->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].total = culture1->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_MARS].total = culture1->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_VENUS].total = culture1->read_i32();
    data.buildings[BUILDING_ORACLE].total = culture1->read_i32();

    // culture 2
    data.buildings[BUILDING_ACTOR_COLONY].total = culture2->read_i32();
    data.buildings[BUILDING_ACTOR_COLONY].active = culture2->read_i32();
    data.buildings[BUILDING_GLADIATOR_SCHOOL].total = culture2->read_i32();
    data.buildings[BUILDING_GLADIATOR_SCHOOL].active = culture2->read_i32();
    data.buildings[BUILDING_LION_HOUSE].total = culture2->read_i32();
    data.buildings[BUILDING_LION_HOUSE].active = culture2->read_i32();
    data.buildings[BUILDING_CHARIOT_MAKER].total = culture2->read_i32();
    data.buildings[BUILDING_CHARIOT_MAKER].active = culture2->read_i32();

    // culture 3
    data.buildings[BUILDING_SMALL_TEMPLE_CERES].active = culture3->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_NEPTUNE].active = culture3->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_MERCURY].active = culture3->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_MARS].active = culture3->read_i32();
    data.buildings[BUILDING_SMALL_TEMPLE_VENUS].active = culture3->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_CERES].active = culture3->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_NEPTUNE].active = culture3->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_MERCURY].active = culture3->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_MARS].active = culture3->read_i32();
    data.buildings[BUILDING_LARGE_TEMPLE_VENUS].active = culture3->read_i32();

    // military
    data.buildings[BUILDING_MILITARY_ACADEMY].total = military->read_i32();
    data.buildings[BUILDING_MILITARY_ACADEMY].active = military->read_i32();
    data.buildings[BUILDING_BARRACKS].total = military->read_i32();
    data.buildings[BUILDING_BARRACKS].active = military->read_i32();

    // support
    data.buildings[BUILDING_MARKET].total = support->read_i32();
    data.buildings[BUILDING_MARKET].active = support->read_i32();
    data.buildings[BUILDING_RESERVOIR].total = support->read_i32();
    data.buildings[BUILDING_RESERVOIR].active = support->read_i32();
    data.buildings[BUILDING_FOUNTAIN].total = support->read_i32();
    data.buildings[BUILDING_FOUNTAIN].active = support->read_i32();
}