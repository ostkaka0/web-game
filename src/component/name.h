#pragma once

#include "core/common.h"
#include "entity_manager.h"

#include <stdio.h>

struct Component_Name {
    char* name;
    bool dealloc;

    void init(u32 id) {}
    void deinit(u32 id) {
        if (dealloc) free(name);
    }
    static void component_init() {}
    static void component_deinit() {}
};