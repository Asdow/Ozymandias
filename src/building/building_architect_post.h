#pragma once

#include "building/building.h"

class building_architect_post : public building_impl {
public:
    building_architect_post(building &b) : building_impl(b) {}
    virtual void spawn_figure() override;
    virtual void window_info_background(object_info &c) override;
};