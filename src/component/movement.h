#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/physics.h"

#include <stdio.h>

enum Movement_Dir {
    MOVEMENT_RIGHT = 1 << 0,
    MOVEMENT_LEFT = 1 << 1,
    MOVEMENT_UP = 1 << 2,
    MOVEMENT_DOWN = 1 << 3
};

Array<u32> g_movement_entities;

struct Ent_Movement {
    u8 dir;

    void init(u32 id) {
        g_movement_entities.heap_insert(id);
    }
    void deinit(u32 id) {
        g_movement_entities.heap_erase(id);
    }
    static void ent_init() {
        g_movement_entities.init();
    }
    static void ent_deinit() {
        g_movement_entities.destroy();
    }
};

void ent_movement_update() {
    for (int i = 0; i < g_movement_entities.length(); i++) {
        u32 entity = g_movement_entities[i];
        auto movement = entity_get<Ent_Movement>(entity);
		auto physics = entity_get<Ent_Physics>(entity);
        assert(movement);
        if (!physics) continue;

        glm::dvec2 dir(0.0);
        if (movement->dir & MOVEMENT_RIGHT) dir.x += 1.0;
        if (movement->dir & MOVEMENT_LEFT) dir.x -= 1.0;
        if (movement->dir & MOVEMENT_UP) dir.y += 1.0;
        if (movement->dir & MOVEMENT_DOWN) dir.y -= 1.0;
        if (glm::length(dir) > 0.0001) dir = glm::normalize(dir);
        else continue;

        g_physics.move_point(physics->id, g_physics.point_pos[physics->id] + dir, 1.0);
    }
}
