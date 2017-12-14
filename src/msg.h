#pragma once

#include <stdio.h>

#include "core\common.h"
#include "world.h"

enum Enum_Msg {
    MSG_NULL,
    MSG_TICK,
    MSG_INIT,

};

struct Msg {
    u16 type;
    union {
        struct {

        } init;
    };
};

Array<Msg> g_msgs = {};
u32 g_msg_tick = 0;

void run(Msg msg) {
    switch (msg.type) {
    case MSG_TICK:
        g_msg_tick++;
        break;
    case MSG_INIT: {
        printf("Message init!\n");
        break;
    }
    default: {

    }
    }
}

void deserialize(void* data, size_t size) {

}

bool msg_tick() {
    int i = 0;
    for (; i < g_msgs.length(); i++) {
        run(g_msgs[i]);
        if (g_msg_tick > g_tick) break;
    }
    g_msgs.splice(0, i);
    return (g_msg_tick > g_tick);
}