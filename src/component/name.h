#pragma once

#include "core/common.h"
#include "entity_manager.h"

#include <stdio.h>

struct Entity_Name {
    char* name;

    void deinit(u32 id) {
        printf("deinit Entity_Name!\n");
    }
};

struct Entity_Name2 {
    char* name;
    void deinit(u32) {};
};

struct Entity_Name3 {
    char* name;
    void deinit(u32) {};
};

