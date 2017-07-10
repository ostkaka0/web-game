#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/name.h"
#include "component/physics.h"
#include "component/sprite.h"

static void entity_player_create(u32 id, glm::dvec2 pos = {0., 0.}, glm::u8vec4 color = {1, 1, 1, 1}) {
    Component_Name name = {"Pertil", false};
    name.name = "Bertil";
    name.dealloc = false;
    component_add(id, name);
    component_add(id, Component_Physics{});
    component_add(id, Component_Movement{0});
    component_add(id, Component_Sprite{color});
    g_physics.move_point(component_get<Component_Physics>(id)->id, pos, 0.1);
}
