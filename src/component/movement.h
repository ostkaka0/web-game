#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/pos.h"

#include <stdio.h>

enum Movement_Dir {
    MOVEMENT_RIGHT = 1 << 0,
    MOVEMENT_LEFT = 1 << 1,
    MOVEMENT_UP = 1 << 2,
    MOVEMENT_DOWN = 1 << 3
};

Array<u32> g_movement_entities;

struct Component_Movement {
    u8 dir;

    void init(u32 id) {
        g_movement_entities.heap_insert(id);
    }
    void deinit(u32 id) {
        g_movement_entities.heap_erase(id);
    }
    static void component_init() {
        g_movement_entities.init();
    }
    static void component_deinit() {
        g_movement_entities.destroy();
    }
};

void component_movement_update() {
    for (int i = 0; i < g_movement_entities.length(); i++) {
        u32 entity = g_movement_entities[i];
        Component_Movement* movement = component_get<Component_Movement>(entity);
        Component_Pos* pos = component_get<Component_Pos>(entity);
        assert(movement);
        if (!pos) continue;

        glm::dvec2 dir(0.0);
        if (movement->dir & MOVEMENT_RIGHT) dir.x += 1.0;
        if (movement->dir & MOVEMENT_LEFT) dir.x -= 1.0;
        if (movement->dir & MOVEMENT_UP) dir.y += 1.0;
        if (movement->dir & MOVEMENT_DOWN) dir.y -= 1.0;
        if (glm::length(dir) > 0.0001) dir = glm::normalize(dir);

        pos->pos += dir;
    }
}
