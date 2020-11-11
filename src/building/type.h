#ifndef int_H
#define int_H

/**
 * @file
 * Type definitions for buildings
 */

/**
 * Building types
 */
enum {
    BUILDING_NONE = 0,
    BUILDING_MENU_FARMS = 2,
    BUILDING_MENU_RAW_MATERIALS = 3,
    BUILDING_MENU_WORKSHOPS = 4, BUILDING_MENU_GUILDS = 4,
    BUILDING_ROAD = 5,
    BUILDING_WALL = 6,
    BUILDING_DRAGGABLE_RESERVOIR = 7, BUILDING_WATER_LIFT = 7,
    BUILDING_AQUEDUCT = 8, BUILDING_IRRIGATION_DITCH = 8,
    BUILDING_CLEAR_LAND = 9,
    BUILDING_HOUSE_VACANT_LOT = 10,
    BUILDING_HOUSE_SMALL_TENT = 10,
    BUILDING_HOUSE_LARGE_TENT = 11,
    BUILDING_HOUSE_SMALL_SHACK = 12,
    BUILDING_HOUSE_LARGE_SHACK = 13,
    BUILDING_HOUSE_SMALL_HOVEL = 14,
    BUILDING_HOUSE_LARGE_HOVEL = 15,
    BUILDING_HOUSE_SMALL_CASA = 16,
    BUILDING_HOUSE_LARGE_CASA = 17,
    BUILDING_HOUSE_SMALL_INSULA = 18,
    BUILDING_HOUSE_MEDIUM_INSULA = 19,
    BUILDING_HOUSE_LARGE_INSULA = 20,
    BUILDING_HOUSE_GRAND_INSULA = 21,
    BUILDING_HOUSE_SMALL_VILLA = 22,
    BUILDING_HOUSE_MEDIUM_VILLA = 23,
    BUILDING_HOUSE_LARGE_VILLA = 24,
    BUILDING_HOUSE_GRAND_VILLA = 25,
    BUILDING_HOUSE_SMALL_PALACE = 26,
    BUILDING_HOUSE_MEDIUM_PALACE = 27,
    BUILDING_HOUSE_LARGE_PALACE = 28,
    BUILDING_HOUSE_LUXURY_PALACE = 29,
    BUILDING_AMPHITHEATER = 30, BUILDING_BANDSTAND = 30,
    BUILDING_THEATER = 31, BUILDING_BOOTH = 31,
    BUILDING_HIPPODROME = 32, BUILDING_SENET_HOUSE = 32,
    BUILDING_COLOSSEUM = 33, BUILDING_PAVILLION = 33,
    BUILDING_GLADIATOR_SCHOOL = 34, BUILDING_CONSERVATORY = 34,
    BUILDING_LION_HOUSE = 35, BUILDING_DANCE_SCHOOL = 35,
    BUILDING_ACTOR_COLONY = 36, BUILDING_JUGGLER_SCHOOL = 36,
    BUILDING_CHARIOT_MAKER = 37,
    BUILDING_PLAZA = 38,
    BUILDING_GARDENS = 39,
    BUILDING_FORT_LEGIONARIES = 40, BUILDING_FORT_CHARIOTEERS = 40,
    BUILDING_SMALL_STATUE = 41,
    BUILDING_MEDIUM_STATUE = 42,
    BUILDING_LARGE_STATUE = 43,
    BUILDING_FORT_JAVELIN = 44, BUILDING_FORT_ARCHERS = 44,
    BUILDING_FORT_MOUNTED = 45, BUILDING_FORT_INFANTRY = 45,
    BUILDING_DOCTOR = 46, BUILDING_APOTHECARY = 46,
    BUILDING_HOSPITAL = 47, BUILDING_MORTUARY = 47,
    BUILDING_BATHHOUSE = 48, BUILDING_MENU_MONUMENTS = 48,
    BUILDING_BARBER = 49, BUILDING_DENTIST = 49,
    BUILDING_DISTRIBUTION_CENTER_UNUSED = 50,
    BUILDING_SCHOOL = 51,
    BUILDING_ACADEMY = 52, BUILDING_MENU_WATER_CROSSINGS = 52,
    BUILDING_LIBRARY = 53,
    BUILDING_FORT_GROUND = 54,
    BUILDING_PREFECTURE = 55, BUILDING_POLICE_STATION = 55,
    BUILDING_TRIUMPHAL_ARCH = 56,
    BUILDING_FORT = 57,
    BUILDING_GATEHOUSE = 58,
    BUILDING_TOWER = 59,
    BUILDING_SMALL_TEMPLE_CERES = 60, BUILDING_TEMPLE_OSIRIS = 60,
    BUILDING_SMALL_TEMPLE_NEPTUNE = 61, BUILDING_TEMPLE_RA = 61,
    BUILDING_SMALL_TEMPLE_MERCURY = 62, BUILDING_TEMPLE_PTAH = 62,
    BUILDING_SMALL_TEMPLE_MARS = 63, BUILDING_TEMPLE_SETH = 63,
    BUILDING_SMALL_TEMPLE_VENUS = 64, BUILDING_TEMPLE_BAST = 64,
    BUILDING_LARGE_TEMPLE_CERES = 65, BUILDING_TEMPLE_COMPLEX_OSIRIS = 65,
    BUILDING_LARGE_TEMPLE_NEPTUNE = 66, BUILDING_TEMPLE_COMPLEX_RA = 66,
    BUILDING_LARGE_TEMPLE_MERCURY = 67, BUILDING_TEMPLE_COMPLEX_PTAH = 67,
    BUILDING_LARGE_TEMPLE_MARS = 68, BUILDING_TEMPLE_COMPLEX_SETH = 68,
    BUILDING_LARGE_TEMPLE_VENUS = 69, BUILDING_TEMPLE_COMPLEX_BAST = 69,
    BUILDING_MARKET = 70,
    BUILDING_GRANARY = 71,
    BUILDING_WAREHOUSE = 72,
    BUILDING_WAREHOUSE_SPACE = 73,
    BUILDING_SHIPYARD = 74,
    BUILDING_DOCK = 75,
    BUILDING_WHARF = 76, BUILDING_FISHING_WHARF = 76,
    BUILDING_GOVERNORS_HOUSE = 77, BUILDING_PERSONAL_MANSION = 77,
    BUILDING_GOVERNORS_VILLA = 78, BUILDING_FAMILY_MANSION = 78,
    BUILDING_GOVERNORS_PALACE = 79, BUILDING_FAMILY_BEEG_PALACE = 79,
    BUILDING_MISSION_POST = 80,
    BUILDING_ENGINEERS_POST = 81,
    BUILDING_LOW_BRIDGE = 82,
    BUILDING_SHIP_BRIDGE = 83, // BUILDING_FERRY = 83,
    BUILDING_SENATE = 84, //BUILDING_VILLAGE_PALACE = 84,
    BUILDING_SENATE_UPGRADED = 85, //BUILDING_TOWN_PALACE = 84,
    BUILDING_FORUM = 86, BUILDING_TAX_COLLECTOR = 86,
    BUILDING_FORUM_UPGRADED = 87, BUILDING_TAX_COLLECTOR_UPGRADED = 87,
    BUILDING_NATIVE_HUT = 88,
    BUILDING_NATIVE_MEETING = 89,
    BUILDING_RESERVOIR = 90, BUILDING_WATER_LIFT2 = 90, //BUILDING_WATER_SUPPLY = 90,
    BUILDING_FOUNTAIN = 91, BUILDING_MENU_BEAUTIFICATION = 91,
    BUILDING_WELL = 92,
    BUILDING_NATIVE_CROPS = 93,
    BUILDING_MILITARY_ACADEMY = 94,
    BUILDING_BARRACKS = 95, BUILDING_RECRUITER = 95,
    BUILDING_MENU_SMALL_TEMPLES = 96, BUILDING_MENU_TEMPLES = 96,
    BUILDING_MENU_LARGE_TEMPLES = 97, BUILDING_MENU_TEMPLE_COMPLEX = 97,
    BUILDING_ORACLE = 98, //BUILDING_FESTIVAL_SQUARE = 98,
    BUILDING_BURNING_RUIN = 99,
    BUILDING_WHEAT_FARM = 100, BUILDING_BARLEY_FARM = 100,
    BUILDING_VEGETABLE_FARM = 101, BUILDING_FLAX_FARM = 101,
    BUILDING_FRUIT_FARM = 102, BUILDING_GRAIN_FARM = 102,
    BUILDING_OLIVE_FARM = 103, BUILDING_LETTUCE_FARM = 103,
    BUILDING_VINES_FARM = 104, BUILDING_POMEGRANATES_FARM = 104,
    BUILDING_PIG_FARM = 105, BUILDING_CHICKPEAS_FARM = 105,
    BUILDING_MARBLE_QUARRY = 106, BUILDING_STONE_QUARRY = 106,
    BUILDING_IRON_MINE = 107, BUILDING_LIMESTONE_QUARRY = 107,
    BUILDING_TIMBER_YARD = 108,
    BUILDING_CLAY_PIT = 109,
    BUILDING_WINE_WORKSHOP = 110, BUILDING_BEER_WORKSHOP = 110,
    BUILDING_OIL_WORKSHOP = 111, BUILDING_LINEN_WORKSHOP = 111,
    BUILDING_WEAPONS_WORKSHOP = 112,
    BUILDING_FURNITURE_WORKSHOP = 113, BUILDING_JEWELS_WORKSHOP = 113,
    BUILDING_POTTERY_WORKSHOP = 114,
    //BUILDING_ROADBLOCK = 115,
    BUILDING_HUNTING_LODGE = 115,
    //
    BUILDING_FERRY = 136,
    //
    BUILDING_ROADBLOCK = 138,
    //
    BUILDING_SHRINE_OSIRIS = 140,
    BUILDING_SHRINE_RA = 141,
    BUILDING_SHRINE_PTAH = 142,
    BUILDING_SHRINE_SETH = 143,
    BUILDING_SHRINE_BAST = 144,
    //
    BUILDING_MENU_SHRINES = 150,
    //
    BUILDING_GOLD_MINE = 161,
    BUILDING_GEMSTONE_MINE = 162,
    //
    BUILDING_FIREHOUSE = 167,
    //
    BUILDING_WALL_PH = 169,
    //
    BUILDING_GATEHOUSE_PH = 171,
    //
    BUILDING_TOWER_PH = 173,
    //
    BUILDING_MENU_DEFENSES = 176,
    BUILDING_CARPENTERS_GUILD = 177,
    BUILDING_BRICKLAYERS_GUILD = 178,
    BUILDING_STONEMASONS_GUILD = 179,
    BUILDING_WATER_SUPPLY = 180,
    BUILDING_TRANSPORT_WHARF = 181,
    BUILDING_WARSHIP_WHARF = 182,
    BUILDING_PYRAMID = 183,
    BUILDING_COURTHOUSE = 184,
    //
    BUILDING_VILLAGE_PALACE = 187,
    BUILDING_TOWN_PALACE = 188,
    BUILDING_CITY_PALACE = 189,
    //
    BUILDING_CATTLE_RANCH = 194,
    BUILDING_REED_GATHERER = 195,
    BUILDING_FIGS_FARM = 196,
    //
    BUILDING_WORK_CAMP = 199,
    //
    BUILDING_GATEHOUSE_PH2 = 202,
    BUILDING_PAPYRUS_WORKSHOP = 203,
    BUILDING_BRICKS_WORKSHOP = 204,
    BUILDING_CHARIOTS_WORKSHOP = 205,
    BUILDING_PHYSICIAN = 206,
    //
    BUILDING_FESTIVAL_SQUARE = 209,
    BUILDING_SPHYNX = 210,
    //
    BUILDING_GRANITE_QUARRY = 216,
    BUILDING_COPPER_MINE = 217,
    //
    BUILDING_SANDSTONE_QUARRY = 221,
    BUILDING_MAUSOLEUM = 222,
    //
    BUILDING_HENNA_FARM = 224,
    BUILDING_ALEXANDRIA_LIBRARY = 225,
    BUILDING_ZOO = 226,
    BUILDING_CAESAREUM = 227,
    BUILDING_PHAROS_LIGHTHOUSE = 228,
    BUILDING_SMALL_ROYAL_TOMB = 229,
    BUILDING_ABU_SIMBEL = 230,
    BUILDING_ARTISANS_GUILD = 231,
    BUILDING_LAMP_WORKSHOP = 232,
    BUILDING_PAINT_WORKSHOP = 233,
    BUILDING_MEDIUM_ROYAL_TOMB = 234,
    BUILDING_LARGE_ROYAL_TOMB = 235,
    BUILDING_GRAND_ROYAL_TOMB = 236,
    //
    int_MAX
};

/**
 * House levels
 */
enum {
    HOUSE_SMALL_TENT = 0,
    HOUSE_LARGE_TENT = 1,
    HOUSE_SMALL_SHACK = 2,
    HOUSE_LARGE_SHACK = 3,
    HOUSE_SMALL_HOVEL = 4,
    HOUSE_LARGE_HOVEL = 5,
    HOUSE_SMALL_CASA = 6,
    HOUSE_LARGE_CASA = 7,
    HOUSE_SMALL_INSULA = 8,
    HOUSE_MEDIUM_INSULA = 9,
    HOUSE_LARGE_INSULA = 10,
    HOUSE_GRAND_INSULA = 11,
    HOUSE_SMALL_VILLA = 12,
    HOUSE_MEDIUM_VILLA = 13,
    HOUSE_LARGE_VILLA = 14,
    HOUSE_GRAND_VILLA = 15,
    HOUSE_SMALL_PALACE = 16,
    HOUSE_MEDIUM_PALACE = 17,
    HOUSE_LARGE_PALACE = 18,
    HOUSE_LUXURY_PALACE = 19,
};

enum {
    BUILDING_STATE_UNUSED = 0,
    BUILDING_STATE_VALID = 1,
    BUILDING_STATE_UNDO = 2,
    BUILDING_STATE_CREATED = 3,
    BUILDING_STATE_RUBBLE = 4,
    BUILDING_STATE_DELETED_BY_GAME = 5, // used for earthquakes, fires, house mergers
    BUILDING_STATE_DELETED_BY_PLAYER = 6,
    BUILDING_STATE_MOTHBALLED = 7
};

#endif // int_H
