#pragma once

#include "figure/figure.h"

class figure_soldier : public figure_impl {
public:
    figure_soldier(figure *f) : figure_impl(f) {}

    virtual void on_create() override {}
    virtual void figure_action() override;
    virtual figure_soldier *dcast_soldier() override { return this; }
    //virtual sound_key phrase_key() const override;
    //virtual bool play_die_sound() override;
    virtual e_overlay get_overlay() const override { return OVERLAY_PAVILION; }
    //virtual figure_sound_t get_sound_reaction(pcstr key) const override;
    virtual void update_image(const formation *m, int &dir);
};

class figure_soldier_infantry : public figure_soldier {
public:
    figure_soldier_infantry(figure *f) : figure_soldier(f) {}

    virtual e_figure_sound phrase() const override { return {FIGURE_INFANTRY, "infantry"}; }
    virtual void update_image(const formation *m, int &dir) override;
};

class figure_soldier_archer : public figure_soldier {
public:
    figure_soldier_archer(figure *f) : figure_soldier(f) {}

    virtual void update_image(const formation *m, int &dir) override;
};

class figure_soldier_charioteer : public figure_soldier {
public:
    figure_soldier_charioteer(figure *f) : figure_soldier(f) {}
    
    virtual void update_image(const formation *m, int &dir) override;
};

class figure_stadard_bearer : public figure_impl {
public:
    figure_stadard_bearer(figure *f) : figure_impl(f) {}

    virtual void figure_action() override;
};