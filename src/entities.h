#pragma once

#include "core/common.h"
#include "ent_manager.h"
#include "ent/name.h"
#include "ent/physics.h"
#include "ent/sprite.h"
#include "ent/move.h"

static void entity_player_cerate(u32 id, glm::dvec2 pos = {0., 0.}, glm::u8vec4 color = {1, 1, 1, 1}) {
    Ent_Name name = {"Pertil", false};
    name.name = "Bertil";
    name.dealloc = false;
    ent_add(id, name);
    ent_add(id, Ent_Physics{});
    ent_add(id, Ent_Move{0});
    ent_add(id, Ent_Sprite{color});
    g_physics.move_point(ent_get(id, Physics)->id, pos, 0.1);
}
