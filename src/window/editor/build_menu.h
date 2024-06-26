#ifndef WINDOW_EDITOR_BUILD_MENU_H
#define WINDOW_EDITOR_BUILD_MENU_H

enum e_editor_build_menu {
    MENU_BRUSH_SIZE = 0,
    MENU_PEOPLE_POINTS = 1,
    MENU_ELEVATION = 2,
    MENU_INVASION_POINTS = 3,
    MENU_RIVER_POINTS = 4,
    MENU_NATIVE_BUILDINGS = 5,
    MENU_ANIMAL_POINTS = 6,
    MENU_NUM_ITEMS = 7
};

void window_editor_build_menu_show(int submenu);

#endif // WINDOW_EDITOR_BUILD_MENU_H
