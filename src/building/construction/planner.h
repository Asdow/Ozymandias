#ifndef BUILDING_CONSTRUCTION_PLANNER_H
#define BUILDING_CONSTRUCTION_PLANNER_H

#include "building/type.h"

#include "graphics/color.h"
#include "map/point.h"
#include "city/view.h"

enum PlannerReqs {
    Groundwater =           1,
    Water =                 1 << 1,
    NearbyWater =           1 << 2,
    ShoreLine =             1 << 3,
    RiverAccess =           1 << 4,
    //
    Meadow =                1 << 6,
    Trees =                 1 << 7,
    Rock =                  1 << 8,
    Ore =                   1 << 9,
    Road =                  1 << 10,
    Intersection =          1 << 11,
    Walls =                 1 << 12,
    //
    Resources =             1 << 16,
    NoEnemyNearby =         1 << 17,
};

enum {
    CAN_PLACE = 0,
    CAN_NOT_PLACE = 1,
    CAN_NOT_BUT_GREEN = 2,
};

extern class BuildPlanner {
private:
    map_point size;
    map_point pivot;
    int tile_graphics_array[30][30] = {};
    int tile_sizes_array[30][30] = {};
    int tile_restricted_terrains[30][30] = {};
    bool tile_blocked_array[30][30] = {};
    int tiles_blocked_total = 0;

    map_point tile_coord_cache[30][30];
    pixel_coordinate pixel_coords_cache[30][30];

    long long requirement_flags = 0;
    int additional_req_param1 = -1;
    int additional_req_param2 = -1;
    int additional_req_param3 = -1;
    int can_place = CAN_PLACE;

    int immediate_warning_id = -1;
    int extra_warning_id = -1;

    int start_offset_x_view;
    int start_offset_y_view;

    void init_tiles(int size_x, int size_y);
    void set_graphics_row(int row, int *image_ids, int total);
    void set_graphics_array(int *image_set, int size_x, int size_y);

    void load_build_graphics(); // fills in data automatically

    void set_tile_size(int row, int column, int size);
    void set_allowed_terrain(int row, int column, int terrain);

    void set_requirements(long long flags, int param1 = -1, int param2 = -1, int param3 = -1);
    void update_obstructions_check();
    void update_requirements_check();
    void dispatch_warnings();

    void update_coord_caches(const map_tile *cursor_tile, int x, int y);
    void draw_flat_tile(int x, int y, color_t color_mask);
    void draw_blueprints(bool fully_blocked);
    void draw_graphics();

    /////

public:
    int building_type;
    bool in_progress;
    bool draw_as_constructing;
    map_tile start;
    map_tile end;
    int total_cost;
    int orientation;
    int variant;

    void reset();
    void load_building(int type);

    void construction_start(int x, int y, int grid_offset);
    void construction_update(int x, int y, int grid_offset);
    void construction_cancel();
    void construction_finalize();

    void update_orientations();
    void construction_record_view_position(int view_x, int view_y, int grid_offset);

    int get_total_drag_size(int *x, int *y);

    void update(const map_tile *cursor_tile);
    void draw();
    bool place();
} Planner;

#endif // BUILDING_CONSTRUCTION_PLANNER_H
