#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/name.h"
#include "component/physics.h"
#include "component/sprite.h"
#include "component/movement.h"

static void entity_player_create(u32 id, glm::dvec2 pos = {0., 0.}, glm::u8vec4 color = {1, 1, 1, 1}) {
    Ent_Name name = {"Pertil", false};
    name.name = "Bertil";
    name.dealloc = false;
    ent_add(id, name);
    ent_add(id, Ent_Physics{});
    ent_add(id, Ent_Movement{0});
    ent_add(id, Ent_Sprite{color});
    g_physics.move_point(entity_get<Ent_Physics>(id)->id, pos, 0.1);
}
