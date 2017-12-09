#pragma once

#include "core/common.h"
#include "entity_manager.h"

#include <stdio.h>

struct Ent_Name {
    char* name;
    bool dealloc;

    void init(u32 id) {}
    void deinit(u32 id) {
        if (dealloc) free(name);
    }
    static void ent_init() {}
    static void ent_deinit() {}
};
