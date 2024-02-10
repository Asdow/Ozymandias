#pragma once

#include "figure/figure.h"

class figure_fireman : public figure_impl {
public:
    figure_fireman(figure *f) : figure_impl(f) {}

    virtual void on_create() override;
    virtual void figure_action() override;
    virtual e_figure_sound phrase() const override { return {FIGURE_FIREMAN, "fireman"}; }
    virtual e_overlay get_overlay() const override { return OVERLAY_WATER; }
    virtual sound_key phrase_key() const override;
    virtual int provide_service() override;

    void extinguish_fire();
    bool fight_fire();
};
