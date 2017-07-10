#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "point_world.h"

#include <stdio.h>

#include <glm/glm.hpp>

Point_World g_physics;

struct Component_Physics {
    u32 id;
    void init(u32 id) {
        this->id = g_physics.add({NAN, NAN}, 1.0);
        printf("\n");
    }
    void deinit(u32 id) {
        g_physics.remove(this->id);
    }
    static void component_init() {
        g_physics.init(1.0e6);
    }
    static void component_deinit() {
        g_physics.deinit();
    }
};
