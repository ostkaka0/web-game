#pragma once

#include "core/common.h"
#include "entity_manager.h"

#include <stdio.h>

#include <glm/glm.hpp>

struct Component_Pos {
    glm::dvec2 pos;
    void init(u32 id) {}
    void deinit(u32 id) {}
    static void component_init() {}
    static void component_deinit() {}
};
