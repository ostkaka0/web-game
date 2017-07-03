#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/name.h"

static void entity_player_create(u32 id) {
    Component_Name name = { "Pertil", false };
    name.name = "Bertil";
    name.dealloc = false;
    component_add(id, name);
    component_add(id, Component_Pos{ glm::dvec2(0.0) });
    component_add(id, Component_Movement{ 0 });
}
