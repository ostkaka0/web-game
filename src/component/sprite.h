#pragma once

#include "core/common.h"
#include "entity_manager.h"
#include "component/physics.h"

#include <SDL.h>
#include <glm/glm.hpp>

Array<u32> g_ent_sprite_entities;

struct Ent_Sprite {
    glm::u8vec4 color;
    void init(u32 id)              { g_ent_sprite_entities.binary_insert(id); }
    void deinit(u32 id)            { g_ent_sprite_entities.binary_erase(id); }
    static void ent_init()   { g_ent_sprite_entities.init(); }
    static void ent_deinit() { g_ent_sprite_entities.destroy(); }
};

void ent_sprite_render(SDL_Renderer* renderer) {
    for (int i = 0; i < g_ent_sprite_entities.length(); i++) {
        u32 entity = g_ent_sprite_entities[i];
        auto sprite = entity_get<Ent_Sprite>(entity);
        assert(sprite);

        if (auto physics = entity_get<Ent_Physics>(entity)) {
            glm::dvec2 pos = g_physics.point_pos[physics->id];
            SDL_Rect fill_rect = { (int)pos.x, (int)-pos.y, 4, 4 };
            SDL_SetRenderDrawColor(renderer, sprite->color.x, sprite->color.y, sprite->color.z, sprite->color.w);
            SDL_RenderFillRect(renderer, &fill_rect);
        }
    }
}

