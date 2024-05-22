#pragma once

#include "animal.h"

class figure_ostrich : public figure_impl {
public:
    figure_ostrich(figure *f) : figure_impl(f) {}

    virtual void on_create() override;
    virtual void figure_action() override;
    virtual e_figure_sound phrase() const override { return {FIGURE_OSTRICH_HUNTER, "hunt_ant"}; }
    virtual void before_poof() override;
    virtual bool play_die_sound() override;

    virtual e_minimap_figure_color minimap_color() const override { return FIGURE_COLOR_ANIMAL; }
};