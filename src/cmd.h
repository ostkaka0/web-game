#pragma once

#include "core\common.h"
#include "core\array.h"
#include "core\bswap.h"
#include "globals.h"

enum Enum_Cmd {
    CMD_NULL = 0,
    CMD_ENT_MOVE,

    CMD_ENT_BEGIN = CMD_ENT_MOVE
};

struct Cmd {
    u16 type;
    union {
        u32 ent;
        struct {
            u32 ent;
            u8 input;
            u16 rel_mouse_x;
            u16 rel_mouse_y;
        } ent_move;
    };
};

Array<Cmd> g_cmds;
u32 g_tick;

void push(Cmd cmd) {
    if (!g_is_server) return;
    g_cmds.push(cmd);
}

void init(Cmd* cmd) {
    switch (cmd->type) {
    }
}

void run(Cmd cmd) {
    switch (cmd.type) {
    case CMD_ENT_MOVE: {
        //set_pos(cmd.ent, cmd.ent_move.pos);
        //set_vel(cmd.ent, cmd.ent_move.vel);
        auto movement = ent_get(cmd.ent, Movement);
        //movement->update_input(cmd.ent_move.input);
        //movement->update_mouse_pos(cmd.ent_move.rel_mouse_pos);
        break;
    }
    default:
        assert(0);
        break;
    }
}

void endian_swap(Cmd* cmd) {
    if (cmd->type >= CMD_ENT_BEGIN) {
        bswap32(&cmd->ent);
    }
    switch (cmd->type) {
    case CMD_ENT_MOVE:
        bswap16(&cmd->ent_move.rel_mouse_x);
        bswap16(&cmd->ent_move.rel_mouse_y);
        break;
    default:
        assert(0);
        break;
    }
}