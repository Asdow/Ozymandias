#pragma once

#include "input/mouse.h"

#include "generic_button.h"

struct arrow_button {
    int x;
    int y;
    int image_id;
    int size;
    void (*click_handler)(int param1, int param2);
    int parameter1;
    int parameter2;
    // state
    int pressed;
    int repeats;

    std::function<void(int,int)> _onclick;

    template<class Func> arrow_button &onclick(Func f) { _onclick = f; return *this; }
};

void arrow_buttons_draw(int x, int y, arrow_button* buttons, int num_buttons, bool tiny = false);
int get_arrow_button(const mouse *m, int x, int y, arrow_button *buttons, int num_buttons);
int arrow_buttons_handle_mouse(const mouse* m, int x, int y, arrow_button* buttons, int num_buttons, int* focus_button_id);

template<class T>
bool arrow_buttons_handle_mouse(const mouse *m, vec2i pos, T &buttons, int &focus_button_id) {
    return buttons.size() > 0 
        ? arrow_buttons_handle_mouse(m, pos.x, pos.y, &buttons.front(), (int)buttons.size(), &focus_button_id)
        : 0;
}

inline void arrow_buttons_draw(vec2i pos, arrow_button &button, bool tiny) {
    arrow_buttons_draw(pos.x, pos.y, &button, 1, tiny);
}