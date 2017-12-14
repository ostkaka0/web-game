#pragma once

#include "core\common.h"

u32 g_tick = 0;

struct {

} g_world = {};

void world_tick() {
    g_tick++;
}