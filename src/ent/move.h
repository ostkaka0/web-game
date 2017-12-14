#pragma once

#include "core/common.h"
#include "ent_manager.h"
#include "ent/physics.h"

#include <stdio.h>

enum Move_Dir {
    MOVE_RIGHT = 1 << 0,
    MOVE_LEFT = 1 << 1,
    MOVE_UP = 1 << 2,
    MOVE_DOWN = 1 << 3
};

Array<u32> g_move_entities;

struct Ent_Move {
    u8 dir;

    void init(u32 id) {
        g_move_entities.heap_insert(id);
    }
    void deinit(u32 id) {
        g_move_entities.heap_erase(id);
    }
    static void ent_init() {
        g_move_entities.init();
    }
    static void ent_deinit() {
        g_move_entities.destroy();
    }
};

void ent_move_update() {
    for (int i = 0; i < g_move_entities.length(); i++) {
        u32 ent = g_move_entities[i];
        auto move = ent_get(ent, Move);
		auto physics = ent_get(ent, Physics);
        assert(move);
        if (!physics) continue;

        glm::dvec2 dir(0.0);
        if (move->dir & MOVE_RIGHT) dir.x += 1.0;
        if (move->dir & MOVE_LEFT) dir.x -= 1.0;
        if (move->dir & MOVE_UP) dir.y += 1.0;
        if (move->dir & MOVE_DOWN) dir.y -= 1.0;
        if (glm::length(dir) > 0.0001) dir = glm::normalize(dir);
        else continue;

        g_physics.move_point(physics->id, g_physics.point_pos[physics->id] + dir, 1.0);
    }
}
