#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/physics.h"

#include <SDL.h>
#include <glm/glm.hpp>

Array<u32> g_component_sprite_entities;

struct Component_Sprite {
    glm::u8vec4 color;
    void init(u32 id)              { g_component_sprite_entities.binary_insert(id); }
    void deinit(u32 id)            { g_component_sprite_entities.binary_erase(id); }
    static void component_init()   { g_component_sprite_entities.init(); }
    static void component_deinit() { g_component_sprite_entities.destroy(); }
};

void component_sprite_render(SDL_Renderer* renderer) {
    for (int i = 0; i < g_component_sprite_entities.length(); i++) {
        u32 entity = g_component_sprite_entities[i];
        Component_Sprite* sprite = component_get<Component_Sprite>(entity);
        assert(sprite);

        if (Component_Physics* physics = component_get<Component_Physics>(entity)) {
            glm::dvec2 pos = g_physics.point_pos[physics->id];
            SDL_Rect fill_rect = { (int)pos.x, (int)-pos.y, 4, 4 };
            SDL_SetRenderDrawColor(renderer, sprite->color.x, sprite->color.y, sprite->color.z, sprite->color.w);
            SDL_RenderFillRect(renderer, &fill_rect);
        }
    }
}

