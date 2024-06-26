#pragma once

struct mouse;

void widget_sidebar_city_draw_background(void);

void widget_sidebar_city_draw_foreground(void);
void widget_sidebar_city_draw_foreground_military(void);

int widget_sidebar_city_handle_mouse(const mouse* m);
int widget_sidebar_city_handle_mouse_build_menu(const mouse* m);

int widget_sidebar_city_get_tooltip_text(void);

void widget_sidebar_city_release_build_buttons();